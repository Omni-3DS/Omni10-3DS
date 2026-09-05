#!/usr/bin/env python3
"""Generate Omni10 icons, banner, splash PNGs and CIA banner WAV."""
import math, os, shutil, struct, zlib, wave

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "assets"))

def chunk(tag, data):
    return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

def write_png(path, w, h, rgb_fn):
    raw = b""
    for y in range(h):
        raw += b"\x00"
        for x in range(w):
            raw += bytes(rgb_fn(x, y))
    ihdr = struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)
    data = chunk(b"IHDR", ihdr) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(b"\x89PNG\r\n\x1a\n" + data)
    print("wrote", path)

def main():
    def icon(x, y):
        cx, cy = 24, 24
        d = (x - cx) ** 2 + (y - cy) ** 2
        if 100 < d < 324:
            return (0, 220, 255)
        return (12, 16, 36)

    def banner(x, y):
        if y < 28:
            return (0, 50, 70)
        if 36 < y < 100 and 20 < x < 236:
            return (0, 190, 230)
        return (10, 14, 30)

    def splash(x, y):
        if y < 28:
            return (0, 200, 255)
        return (12, 16, 40) if ((x // 40 + y // 40) % 2 == 0) else (14, 18, 44)

    write_png(os.path.join(ROOT, "icons", "icon.png"), 48, 48, icon)
    write_png(os.path.join(ROOT, "icons", "icon-256.png"), 256, 256,
              lambda x, y: icon(x * 48 // 256, y * 48 // 256))
    write_png(os.path.join(ROOT, "banner", "banner.png"), 256, 128, banner)
    write_png(os.path.join(ROOT, "splash", "splash_top.png"), 400, 240, splash)
    # Universal-Updater convenience path
    shutil.copyfile(os.path.join(ROOT, "icons", "icon.png"), os.path.join(ROOT, "icon.png"))
    print("wrote", os.path.join(ROOT, "icon.png"))

    wav_path = os.path.join(ROOT, "audio", "banner.wav")
    os.makedirs(os.path.dirname(wav_path), exist_ok=True)
    fr, n = 22050, 22050 // 2
    with wave.open(wav_path, "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(fr)
        frames = bytearray()
        for i in range(n):
            t = i / fr
            v = int(9000 * math.sin(2 * math.pi * 523.25 * t)) if t < 0.12 else 0
            frames += struct.pack("<h", v)
        w.writeframes(frames)
    print("wrote", wav_path)
    print("assets ready under", ROOT)

if __name__ == "__main__":
    main()
