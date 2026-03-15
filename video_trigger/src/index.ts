import express from "express";
import fs from "fs";
import { exec } from "child_process";
import { v4 as uuidv4 } from "uuid";
import path from "path";
import { uploadToS3 , listFoldersInVideodata} from "./s3";
import { startRollingBuffer } from "./record";
import { BUFFER_DIR } from "./config";
import cors from "cors";


const app = express();
app.use(cors());
const PORT = 3000;
let uid: string | null = null;

startRollingBuffer();

app.get("/getid", (req, res) => {
    // Simply return the videoId that was generated earlier (if exists) or null
    res.json({ id: uid }); 
});

app.get("/getfolder", (req, res) => {
  // Simply return the videoId that was generated earlier (if exists) or null

  listFoldersInVideodata().then((folders) => {
    res.json(folders);
  });
  
});
app.get("/start", async (req, res) => {
  console.log("STARTING");
  res.sendStatus(200);
  res.send("OK");
})
app.post("/stop", async (req, res) => {
  console.log("STOPPING");
  res.send("OK");
  
})
app.post("/upload", async (req, res) => {
  // await stopRollingBuffer();
   uid = uuidv4();
  const tempFile = `temp_${uid}.txt`;
  const finalVideo = `final_${uid}.mp4`;


  

  const parts = fs.readdirSync(BUFFER_DIR)
    .filter(f => f.endsWith(".mp4"))
    .sort()
    .map(f => `file '${path.join(BUFFER_DIR, f).replace(/\\/g, "/")}'`)
    .slice(0, -1)
    .join("\n");

  fs.writeFileSync(tempFile, parts);

  console.log(tempFile);
  console.log(parts);

  exec(`ffmpeg -f concat -safe 0 -i ${tempFile} -c copy ${finalVideo}`, async (err) => {
    if (err) {
      console.error("Stitching failed:", err);
      return res.status(500).send("Failed to create video");
    }

    await uploadToS3(finalVideo, uid);

    fs.unlinkSync(tempFile);
    fs.unlinkSync(finalVideo);

    return res.json({ success: true, id: uid });
  });
});

app.listen(PORT, () => {
  console.log(`🚀 Server listening on http://localhost:${PORT}`);
});
