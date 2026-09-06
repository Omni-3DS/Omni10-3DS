#!/usr/bin/env python3
"""After decode_firm_main.py — force real SDMMC linkage in generated main.c"""
from pathlib import Path
import re
import sys

root = Path(__file__).resolve().parents[1]
main = root / "firm" / "arm9" / "main.c"
if not main.is_file():
    print("no main.c")
    sys.exit(1)
t = main.read_text()

# Replace soft battery-based sdmmc_probe if still present
soft = re.compile(
    r"static void sdmmc_probe\(void\)\{\s*battery_probe\(\);\s*g_sd_ok = \(g_battery >= 0\) \? 1 : 0;\s*\}",
    re.S,
)
real = r'''/* Real hardware from sdmmc.c */
extern int sdmmc_init(void);
extern int sdmmc_card_inserted(void);
extern int fat_read_mbr(unsigned char *sector512);
static void sdmmc_probe(void){
        if(!sdmmc_card_inserted()){ g_sd_ok=0; return; }
        g_sd_ok = (sdmmc_init()==0) ? 1 : 0;
}'''
if soft.search(t):
    t = soft.sub(real, t, count=1)
    print("replaced soft sdmmc_probe")
elif "sdmmc_card_inserted" not in t:
    # inject before first sdmmc_probe
    if "static void sdmmc_probe" in t:
        t = t.replace("static void sdmmc_probe", real.replace("static void sdmmc_probe", "static void sdmmc_probe_UNUSED") + "\nstatic void sdmmc_probe", 1)
        # simpler: just prepend externs near g_sd_ok
        t = t.replace("static int g_sd_ok = -1;", "static int g_sd_ok = -1;\n" + real, 1)
        print("injected real probe block")
    else:
        print("WARN no sdmmc_probe")
else:
    print("real sd already present")

main.write_text(t)
print("patched", main, len(t))
