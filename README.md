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
├── .github/workflows/     # GitHub Actions CI
├── assets/
│   └── splash/            # Splash screen images & logo
├── docs/
│   ├── lua-api.md         # Full Lua API documentation
│   ├── o10-scripting.md   # .o10 language documentation
│   └── ROADMAP.md         # Development roadmap
├── include/               # Header files
├── scripts/
│   └── examples/          # Example .o10 and .lua scripts
├── source/                # Actual source code (coming soon)
├── Makefile
├── LICENSE                # GPL-3.0
└── README.md
```

## Scripting

### Native `.o10` Scripts
High-level, easy to read automation language.  
→ See [docs/o10-scripting.md](docs/o10-scripting.md)

### Lua 5.4
Full power with a clean and extended API (`ui`, `fs`, `sys`, `net`, `nand`, `title`…).  
→ See [docs/lua-api.md](docs/lua-api.md)

Example scripts are available in `scripts/examples/`.

## Building

**Requirements:**
- [devkitARM](https://devkitpro.org/)
- firmtool
- Python 3

```bash
make firm
```

GitHub Actions automatically builds on every push to `main` / `develop`.

## Status

**Early development / project skeleton**

- Repository & CI ✓
- Documentation ✓
- Example scripts ✓
- Splash assets folder ✓
- Real source code → in progress

## License

GPL-3.0

## Credits

Inspired by the excellent work of **d0k3** and the entire 3DS homebrew community.
