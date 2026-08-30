# Omni10-3DS

**Full Access. No Limits.**

Omni10-3DS is a next-generation file browser, multi-tool and scripting environment for the Nintendo 3DS family.

It aims to be significantly more powerful and modern than GodMode9 while keeping the same level of deep system access.

## Key Goals

- Native high-level scripting language (`.o10`)
- Powerful and clean Lua API
- **Real FTP server + client** support
- Modern, themeable UI
- Custom splash screen & branding
- Better safety features and logging
- ARM9 + ARM11 cooperation for advanced features

## Project Structure

```
Omni10-3DS/
├── .github/workflows/        # GitHub Actions CI
├── assets/splash/            # Splash screen assets
├── data/                     # Bundled assets (embedded at build)
├── docs/
│   ├── lua-api.md            # Lua API documentation
│   ├── o10-scripting.md      # .o10 language documentation
│   └── ROADMAP.md
├── include/
│   ├── common.h
│   ├── ui.h
│   ├── fs.h
│   ├── net.h                 # FTP support
│   └── o10.h
├── source/
│   ├── main.c
│   ├── ui.c
│   ├── fs.c
│   ├── net.c
│   └── o10.c
├── scripts/examples/         # Example .o10 + .lua scripts
├── Makefile
├── CONTRIBUTING.md
├── LICENSE                   # GPL-3.0
└── README.md
```

## Scripting

### Native `.o10` Scripts
High-level, easy to read automation language.  
→ [docs/o10-scripting.md](docs/o10-scripting.md)

### Lua 5.4
Full power with clean modules: `ui`, `fs`, `sys`, `net`, `nand`, `title`…  
→ [docs/lua-api.md](docs/lua-api.md)

Example scripts are in `scripts/examples/`.

## Building

```bash
make firm
```

**Requirements:** devkitARM + firmtool + Python 3

GitHub Actions builds automatically on every push.

## Current Status

| Component              | Status          |
|------------------------|-----------------|
| Project structure      | Done            |
| Headers & stubs        | Done            |
| UI / FS / Net / o10    | Stubs ready     |
| Documentation          | Done            |
| Example scripts        | Done            |
| Splash folder          | Ready           |
| Real FIRM code         | Next step       |
| FTP implementation     | Planned         |

## License

GPL-3.0

## Credits

Inspired by the excellent work of **d0k3** and the entire 3DS homebrew community.
