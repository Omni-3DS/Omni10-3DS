# Omni10-3DS

**Full Access. No Limits.**

Custom Nintendo 3DS FIRM — Luma3DS chainloader, Old 3DS + New 3DS / New 2DS XL.

## Status (v0.3.0)

| Feature | State |
|---------|--------|
| Boot under Luma | Working |
| RGB888 top + bottom screen | Working |
| Main menu + text UI | Working |
| EN / DE language | Working |
| Settings (language toggle) | Working |
| Confirm dialogs (reboot / power) | Working |
| **.o10 demo runner** | Working (built-in scripts) |
| System info (O3DS/N3DS) | Working |
| START+SELECT power off | Working |
| File browser / real FTP / SD scripts | Next |

## Install

1. Download `Omni10.firm` from [Actions](https://github.com/Omni-3DS/Omni10-3DS/actions)  
2. `sd:/luma/payloads/Omni10.firm`  
3. Hold **START** on boot → **Omni10**

## Controls

| Input | Action |
|-------|--------|
| D-Pad | Navigate |
| **A** | Select / Yes |
| **B** | Back / No |
| **START + SELECT** | Power off |

## .o10 (built-in demos)

```
PRINT HELLO FROM O10
WAIT
PRINT OMNI10 RULES
WAIT
PRINT DONE
```

Commands: `PRINT`, `WAIT`, `POWEROFF`, `REBOOT`

## Build

```bash
make firm   # needs devkitARM + firmtool
```

## License

GPL-3.0
