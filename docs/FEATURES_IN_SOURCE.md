# Features that ARE in `firm/arm9/main.c` (v0.8.x)

Open this file in the repo:

**https://github.com/Omni-3DS/Omni10-3DS/blob/main/firm/arm9/main.c**

Size: **~29 298 bytes** (not the old placeholder).

## Menu (12 items)

1. ABOUT / INFO  
2. SYSTEM INFO  
3. **HOME SCRIPTS** ← o10 / SLR / Lua hub  
4. SETTINGS (EN/DE)  
5. INTERNET / WIFI  
6. BATTERY INFO  
7. BUTTON TEST  
8. LED TEST  
9. FILE BROWSER  
10. FTP  
11. REBOOT  
12. POWER OFF  

## Buttons

| Button | Action |
|--------|--------|
| **X** | Open **HOME SCRIPTS** (quick) |
| A | Select menu |
| B | Back |
| START+SELECT | Power off |

> Physical **HOME** is often **not** readable in a Luma FIRM payload (Home Menu owns it).  
> Omni10 uses **X** as the HOME-scripts shortcut.

## Search in main.c

```
screen_scripts_hub
HOME SCRIPTS
screen_filebrowser
screen_ftp
screen_battery
BTN_X
```

## Install new firm

1. GitHub Actions → download **Omni10.firm** from latest green build  
2. Or: Installer **Update Omni10** (downloads latest release)  
3. Copy to `sdmc:/luma/payloads/Omni10.firm`  
4. Hold **START** on boot → select Omni10  

If you still only see the old menu, you are running an **old** `.firm` on the SD — not the current source.
