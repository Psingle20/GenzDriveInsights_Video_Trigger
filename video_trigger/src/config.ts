import dotenv from "dotenv";
dotenv.config();

export const BUCKET_NAME = process.env.BUCKET_NAME!;
export const REGION = process.env.REGION!;
export const ACCESS_KEY = process.env.ACCESS_KEY!;
export const SECRET_KEY = process.env.SECRET_KEY!;
export const BUFFER_DIR = "./buffer";
