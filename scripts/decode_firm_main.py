#!/usr/bin/env python3
import base64, pathlib, zlib, sys
root = pathlib.Path(__file__).resolve().parents[1]
parts = []
for i in range(4):
    p = root / "scripts" / f"firm_main_{i}.zlib.b64"
    parts.append(p.read_text().replace("\n", "").replace(" ", "").strip())
b64 = "".join(parts)
data = zlib.decompress(base64.b64decode(b64))
out = root / "firm" / "arm9" / "main.c"
out.write_bytes(data)
print(f"wrote {out} ({len(data)} bytes)")
for tag in (b"int main", b"screen_filebrowser", b"screen_ftp", b"screen_scripts_hub", b"FULL FEATURES", b"HOME SCRIPTS"):
    print(tag.decode(), "OK" if tag in data else "MISSING")
sys.exit(0 if b"FULL FEATURES" in data and b"int main" in data else 1)
