# RTSP FFmpeg Segmenter (C++)

A tiny C++ wrapper that **forks + detaches** and then `execv()`s `/usr/bin/ffmpeg` to record an **RTSP** stream into **timestamped `.ts` segments** (10s each).

## Requirements

- Linux (uses `fork()`, `setsid()`, `execv()`)
- `g++` with C++17 support
- FFmpeg installed at **`/usr/bin/ffmpeg`**

Check:

```bash
/usr/bin/ffmpeg -version
g++ --version
```

## Build

```bash
cd /home/odmin/Documents/Workspace/C_FFMPEG_RTSP
make
```

Output binary: `rtsp_segmenter`

Clean:

```bash
make clean
```

## Usage

```bash
./rtsp_segmenter <rtsp_url> <segment_dir>
```

Example:

```bash
mkdir -p logs /tmp/segments
./rtsp_segmenter "rtsp://user:pass@192.168.1.10:554/stream1" "/tmp/segments"
```

Segments are written like:

```text
/tmp/segments/segment_2026-01-28T14-05-10.ts
```

## Logging

Stdout/stderr from FFmpeg are redirected to:

- `logs/out.log`
- `logs/err.log`

Make sure `logs/` exists (the program does not create it automatically):

```bash
mkdir -p logs
```

## Notes / Gotchas

- **Video/audio are stream-copied** (`-c:v copy -c:a copy`). Your camera/stream codecs must be compatible with MPEG-TS.
- The process **returns immediately** in the parent after forking; it prints the FFmpeg PID.
- The segment duration is currently **10 seconds** (`-segment_time 10`).
- If your FFmpeg is not at `/usr/bin/ffmpeg`, update `main.cpp` (variable `ffmpeg`) or create a symlink.


