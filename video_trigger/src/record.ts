import { spawn, ChildProcessWithoutNullStreams } from "child_process";
import { existsSync, mkdirSync } from "fs";
import { BUFFER_DIR } from "./config";

let ffmpeg: ChildProcessWithoutNullStreams | null = null;


export function startRollingBuffer(): void {
  if (ffmpeg) {
    console.log("⚠️ Rolling buffer already running.");
    return;
  }

  if (!existsSync(BUFFER_DIR)) mkdirSync(BUFFER_DIR);

  const args = [
    "-f", "dshow", // Windows input format
    "-i", "video=Integrated Camera", // Replace with your actual webcam name
    "-vf", "scale=640:360",
    "-c:v", "libx264",
    "-f", "segment",
    "-segment_time", "30",
    "-segment_wrap", "10",
    "-reset_timestamps", "1",
    `${BUFFER_DIR}/part_%02d.mp4`
  ];

  ffmpeg = spawn("ffmpeg", args);

  ffmpeg.stderr.on("data", (data) => {
    console.log(`[FFMPEG]: ${data}`);
  });

  ffmpeg.on("exit", (code) => {
    console.log(`🛑 FFMPEG exited with code ${code}`);
    ffmpeg = null;
  });

  console.log("🎥 Rolling buffer started...");
}

export function stopRollingBuffer(): Promise<void> {
  return new Promise((resolve, reject) => {
    if (!ffmpeg) {
      console.log("⚠️ No ffmpeg process running.");
      return resolve();
    }

    ffmpeg.once("exit", () => resolve());

    try {
      ffmpeg.kill("SIGINT");
    } catch (err) {
      reject(err);
    }
  });
}
