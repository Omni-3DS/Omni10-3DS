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

## Status

**Very early development / planning stage.**

This repository currently contains the project skeleton, documentation and CI setup. The actual firmware code is being developed.

## Scripting

- Primary scripting format: **`.o10`**
- Full Lua 5.4 support with an extended API
- Easy automation of complex tasks

## Building

Building requires:

- [devkitARM](https://devkitpro.org/)
- firmtool
- Python 3

```bash
make firm
```

GitHub Actions CI is set up to automatically build on every push (once the full source is present).

## License

GPL-3.0 (same spirit as GodMode9 – free and open source)

## Credits

Inspired by the excellent work of d0k3 and the entire 3DS homebrew community.
