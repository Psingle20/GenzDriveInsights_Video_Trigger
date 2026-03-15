import { S3Client, PutObjectCommand , ListObjectsV2Command} from "@aws-sdk/client-s3";
import fs from "fs";
import path from "path";
import { BUCKET_NAME, REGION, ACCESS_KEY, SECRET_KEY } from "./config";

const s3 = new S3Client({
  region: REGION,
  endpoint: "https://2f23de4b98ddafd57ec70554794c0e1b.r2.cloudflarestorage.com",
  credentials: {
    accessKeyId: ACCESS_KEY,
    secretAccessKey: SECRET_KEY,
  },
});

export async function uploadToS3(filePath: string, key: string|null) {
  const fileContent = fs.readFileSync(filePath);
  const uploadParams = {
    Bucket: BUCKET_NAME,
    Key: `videodata/${key}/footage.mp4`,
    Body: fileContent,
    ContentType: "video/mp4",
  };

  await s3.send(new PutObjectCommand(uploadParams));
  console.log(`✅ Uploaded to S3: ${key}`);
}
export async function listFoldersInVideodata() {
  const command = new ListObjectsV2Command({
    Bucket: BUCKET_NAME,
    Prefix: "videodata/",
    Delimiter: "/"
  });

  const response = await s3.send(command);

  const folders = response.CommonPrefixes?.map(prefix => prefix.Prefix?.replace("videodata/", "").replace("/", "")) || [];

  console.log("Folder IDs inside videodata/:", folders);
  return folders;
}
