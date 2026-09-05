#!/usr/bin/env python3
"""Decode firm/arm9/main.c from scripts/firm_main.zlib.b64 (full features)."""
import base64, pathlib, zlib, sys

root = pathlib.Path(__file__).resolve().parents[1]
b64_path = root / "scripts" / "firm_main.zlib.b64"
out = root / "firm" / "arm9" / "main.c"

if not b64_path.is_file():
    print("missing", b64_path)
    sys.exit(1)

b64 = b64_path.read_text().strip()
data = zlib.decompress(base64.b64decode(b64))
out.write_bytes(data)
print(f"wrote {out} ({len(data)} bytes)")
for tag in (b"int main", b"screen_filebrowser", b"screen_ftp", b"screen_scripts_hub", b"OMNI_VERSION"):
    print(tag.decode(), "OK" if tag in data else "MISSING")
sys.exit(0 if b"int main" in data else 1)
