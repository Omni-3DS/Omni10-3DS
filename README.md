# Omni10-3DS

**Full Access. No Limits.**

Custom Nintendo **3DS FIRM** for **Luma3DS** (Old / New 3DS / New 2DS XL).

## Install

1. Get `Omni10.firm` from [Actions](https://github.com/Omni-3DS/Omni10-3DS/actions)
2. `sd:/luma/payloads/Omni10.firm`
3. Hold **START** on boot → **Omni10**

## Controls

| Input | Action |
|-------|--------|
| D-Pad | Navigate |
| A / B | Select / Back |
| **X** | **HOME Scripts** (.o10 / .slr / .lua) |
| START+SELECT | Power off |

## Highlights (v0.6)

- Status bar: **NET** + **BATTERY %** (MCU)
- System info, language EN/DE
- HOME Scripts hub (O10 / SLR / Lua)
- Battery info, button test, LED test
- Internet / WiFi MCU test
- Reboot / power off
- o10script design: compile **only on 3DS** → opaque `.o10`

Full list: [docs/FEATURES.md](docs/FEATURES.md)

## o10script

```text
sdmc:/o10/slr/*.slr  →  compile on console  →  *.o10  →  run
```

No PC compile for console `.o10`. Docs: [O10-SLR](https://github.com/Omni-3DS/O10-SLR)

## Build

```bash
make firm   # devkitARM + firmtool
```

## License

GPL-3.0
