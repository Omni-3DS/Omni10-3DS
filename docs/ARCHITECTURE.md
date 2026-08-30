# Omni10-3DS Architecture

## Overview

Omni10-3DS is designed as a dual-core FIRM payload for the Nintendo 3DS.

```
┌─────────────────────────────────────────────────────┐
│                    Omni10.firm                       │
├──────────────────────┬──────────────────────────────┤
│       ARM9           │           ARM11              │
│                      │                              │
│  - File System       │  - UI / Framebuffer          │
│  - Crypto            │  - Networking / FTP          │
│  - NAND Access       │  - Input handling            │
│  - Low-level IO      │  - Higher-level logic        │
│  - .o10 + Lua runtime│  - Themes / Splash           │
└──────────────────────┴──────────────────────────────┘
```

## Design Goals

1. **Clean separation** between ARM9 (privileged, low-level) and ARM11 (UI + network)
2. **Real networking** – FTP server runs on ARM11 where sockets are possible
3. **Modern scripting** – both `.o10` (simple) and Lua 5.4 (powerful)
4. **Safety first** – clear unlock sequences, logging, confirmations
5. **Extensible** – module system for future features

## Core Modules

| Module     | Core | Responsibility                          |
|------------|------|-----------------------------------------|
| `ui`       | 11   | Drawing, menus, dialogs, progress bars  |
| `fs`       | 9    | SD + NAND file system access            |
| `net`      | 11   | FTP server/client, network status       |
| `o10`      | 9/11 | Native script parser & runtime          |
| `lua`      | 9/11 | Lua 5.4 interpreter + bindings          |
| `nand`     | 9    | Raw NAND operations, backups            |
| `title`    | 9    | CIA / Title handling                    |
| `crypto`   | 9    | AES, SHA, signatures                    |
| `input`    | 11   | Buttons, touch, circle pad              |
| `splash`   | 11   | Boot splash rendering                   |

## Boot Flow (planned)

1. FIRM is loaded by boot9strap / Luma / fastboot3DS
2. ARM9 initializes hardware & crypto
3. ARM11 is started
4. Splash screen is shown
5. Filesystems are mounted
6. Main UI or autorun script starts

## Scripting Layers

```
User Script (.o10 or .lua)
        ↓
   Script Engine
        ↓
   Omni10 API (ui, fs, net, ...)
        ↓
   Hardware / System
```

## FTP Architecture

Unlike classic GodMode9 (mostly ARM9), Omni10 will run the FTP server on **ARM11** so real TCP sockets can be used. ARM9 only handles the actual file read/write requests via IPC.
