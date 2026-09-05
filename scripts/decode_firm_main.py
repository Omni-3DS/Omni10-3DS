#!/usr/bin/env python3
"""Decode firm/arm9/main.c from scripts/firm_main.zlib.b64"""
import base64, pathlib, zlib, sys
root = pathlib.Path(__file__).resolve().parents[1]
b64 = (root / "scripts" / "firm_main.zlib.b64").read_text().strip()
out = root / "firm" / "arm9" / "main.c"
data = zlib.decompress(base64.b64decode(b64))
out.write_bytes(data)
print(f"wrote {out} ({len(data)} bytes)")
sys.exit(0 if b"int main" in data else 1)
