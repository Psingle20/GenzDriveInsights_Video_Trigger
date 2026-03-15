# Video Streaming Blackbox

A vehicle blackbox / dashcam system that continuously records video, detects crash events via an Arduino sensor module, uploads footage to cloud storage, and provides a React dashboard for playback and drive analytics.

---

## Architecture Overview

```
Arduino (MPU6050 + SIM808)
        │  crash/motion event → HTTP trigger
        ▼
video_trigger  ──────────────────────────────┐
(rolling webcam buffer via FFmpeg)           │ upload footage.mp4
                                             ▼
                                  Cloudflare R2  (S3-compatible)
                                             │
                                             ▼
                                      Serve_Video
                                  (reverse proxy, port 8000)
                                             │
                                             ▼
                               GenZ-DriveInsights (React)
                        (auth, video playback, maps, charts)
```

---

## Components

### 1. `video_trigger` — Capture & Upload Service

Node.js / TypeScript Express server (port **3000**) that runs on the recording device (Windows).

**What it does:**
- Starts a **rolling 30-second FFmpeg segment buffer** on boot, keeping the last 10 segments (~5 minutes) in `./buffer/`
- On `POST /upload` — stitches buffered segments into a single MP4 using FFmpeg and uploads it to Cloudflare R2 under a unique UUID key
- `GET /getid` — returns the UUID of the last uploaded clip
- `GET /getfolder` — lists all clip folders stored in R2

**Key files:**
| File | Description |
|------|-------------|
| `src/index.ts` | Express routes and upload logic |
| `src/record.ts` | FFmpeg rolling buffer (dshow / Integrated Camera) |
| `src/s3.ts` | Cloudflare R2 upload and folder listing |
| `src/config.ts` | Loads `.env` credentials |

**Environment variables (`.env`):**
```
BUCKET_NAME=videodata
REGION=auto
ACCESS_KEY=<cloudflare-r2-access-key>
SECRET_KEY=<cloudflare-r2-secret-key>
```

> ⚠️ **Never commit your `.env` file.** It is already listed in `.gitignore`.

**Setup & run:**
```bash
cd video_trigger
npm install
npm run dev       # or: npx ts-node src/index.ts
```

**Prerequisites:**
- [FFmpeg](https://ffmpeg.org/download.html) installed and available in `PATH`
- A webcam accessible as `video=Integrated Camera` (update `record.ts` if your device name differs)

---

### 2. `Serve_Video` — Reverse Proxy

Node.js / TypeScript Express server (port **8000**) that acts as a reverse proxy to stream stored videos from Cloudflare R2.

**How it works:**
- Reads the **subdomain** from the incoming request hostname (e.g. `<uuid>.yourdomain.com`)
- Proxies the request to `https://<r2-public-url>/videodata/<uuid>/footage.mp4`
- Appends `footage.mp4` automatically for root (`/`) requests

**Setup & run:**
```bash
cd Serve_Video
npm install
npm run dev       # or: npx ts-node src/index.ts
```

---

### 3. `Frontend/GenZ-DriveInsights` — React Dashboard

A React web application providing:
- **Authentication** — Firebase-based login / signup
- **Home dashboard** — overview of recorded trips
- **Map view** — GPS route visualization
- **Charts** — drive analytics (speed, acceleration, etc.)

**Setup & run:**
```bash
cd "Frontend/GenZ-DriveInsights/REACT APP"
npm install
npm start
```

Configure Firebase credentials in `src/base.js`.

---

### 4. Arduino Hardware

Located in `Frontend/GenZ-DriveInsights/arduino/`

**Components:**
| Component | Role |
|-----------|------|
| MPU6050 | Accelerometer / gyroscope — detects crashes and harsh movements |
| SIM808 | GSM + GPS module — provides location data and sends SMS / HTTP alerts |

**How it works:**
- Continuously reads acceleration and gyroscope data from the MPU6050
- On crash/motion detection, triggers the `video_trigger` upload endpoint via the SIM808's HTTP capability
- GPS coordinates are read from the SIM808 and sent to the dashboard

**Flash** the `.ino` files using the [Arduino IDE](https://www.arduino.cc/en/software).

---

## Getting Started

### Prerequisites
- Node.js >= 18
- npm
- FFmpeg (for `video_trigger`)
- Arduino IDE (for hardware)
- Cloudflare R2 bucket (or any S3-compatible storage)

### Quick Start

1. **Clone the repo**
   ```bash
   git clone <your-repo-url>
   cd Video_STreaming_blackbox
   ```

2. **Configure environment**
   ```bash
   cp video_trigger/.env.example video_trigger/.env
   # Fill in your Cloudflare R2 credentials
   ```

3. **Start the capture service**
   ```bash
   cd video_trigger && npm install && npm run dev
   ```

4. **Start the video proxy**
   ```bash
   cd Serve_Video && npm install && npm run dev
   ```

5. **Start the dashboard**
   ```bash
   cd "Frontend/GenZ-DriveInsights/REACT APP" && npm install && npm start
   ```

---

## Project Structure

```
Video_STreaming_blackbox/
├── video_trigger/          # Capture & upload service (port 3000)
│   ├── src/
│   │   ├── index.ts        # Express API
│   │   ├── record.ts       # FFmpeg rolling buffer
│   │   ├── s3.ts           # Cloudflare R2 client
│   │   └── config.ts       # Environment config
│   └── buffer/             # Temporary video segments (gitignored)
│
├── Serve_Video/            # Reverse proxy server (port 8000)
│   └── src/
│       └── index.ts
│
└── Frontend/
    └── GenZ-DriveInsights/
        ├── arduino/        # Arduino firmware (MPU6050 + SIM808)
        └── REACT APP/      # React dashboard
            └── src/
```

---

## Security Notes

- `.env` files are **gitignored** — never commit credentials
- Rotate your Cloudflare R2 keys if they have been exposed
- The `buffer/` directory is gitignored to avoid committing raw video data
