# FIRM source (full features)

The ARM9 payload source is **~30KB** and lives as compressed data:

```text
scripts/firm_main.zlib.b64   # compressed main.c
scripts/decode_firm_main.py  # writes firm/arm9/main.c
```

Build:

```bash
python3 scripts/decode_firm_main.py   # or: make firm (calls decode)
make firm
```

## Features in this main.c

- About / version from `version.dat` (`OMNI_VERSION`)
- System info, battery %, WiFi status
- HOME Scripts hub (.o10 / .slr / lua paths)
- Internet / WiFi screen
- Button test, LED test
- File browser UI, FTP UI
- Reboot / power off
- EN/DE language

If `scripts/firm_main.zlib.b64` is missing, Makefile falls back to the old 0.3.10 restore and only rewrites version strings.
