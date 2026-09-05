#!/usr/bin/env python3
"""Decode firm/arm9/main.c from scripts/firm_main_{0..3}.zlib.b64"""
import base64
import pathlib
import sys
import zlib

root = pathlib.Path(__file__).resolve().parents[1]
parts = []
for i in range(4):
    p = root / "scripts" / f"firm_main_{i}.zlib.b64"
    if not p.is_file():
        print("MISSING", p)
        sys.exit(1)
    parts.append(p.read_text().replace("\n", "").replace(" ", "").strip())

b64 = "".join(parts)
data = zlib.decompress(base64.b64decode(b64))
out = root / "firm" / "arm9" / "main.c"
out.write_bytes(data)
print(f"wrote {out} ({len(data)} bytes)")

required = (
    b"int main",
    b"screen_filebrowser",
    b"screen_ftp",
    b"screen_scripts_hub",
    b"HOME SCRIPTS",
)
ok = True
for tag in required:
    present = tag in data
    print(tag.decode(), "OK" if present else "MISSING")
    if not present:
        ok = False

sys.exit(0 if ok else 1)
