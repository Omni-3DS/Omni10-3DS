#!/usr/bin/env python3
"""Wire real SDMMC + FAT32 + O10BK1 backup into generated main.c"""
from pathlib import Path
import re
import sys

root = Path(__file__).resolve().parents[1]
main = root / "firm" / "arm9" / "main.c"
if not main.is_file():
    print("no main.c")
    sys.exit(1)
t = main.read_text()

EXTERN = r'''
/* === real storage API (sdmmc.c / fat32.c) — always visible === */
extern int sdmmc_init(void);
extern int sdmmc_card_inserted(void);
extern int sdmmc_is_sdhc(void);
extern unsigned int sdmmc_get_sectors(void);
extern int sdmmc_readsectors(unsigned int sector, unsigned int count, unsigned char *out);
extern int fat_read_mbr(unsigned char *sector512);
extern int o10_backup_write(const unsigned char *payload, unsigned int len, unsigned int *out_lba);
extern int o10_backup_read(unsigned char *payload, unsigned int max_len, unsigned int *out_len);
'''

# Remove any previous injection stuck inside ifndef
t = re.sub(
    r"/\* === real storage API.*?\*/\n(?:extern [^;]+;\n)+",
    "",
    t,
    count=1,
    flags=re.S,
)

if "o10_backup_write" not in t or "real storage API" not in t:
    # Prefer right after the OMNI_VERSION ifndef/endif block
    m = re.search(
        r"#ifndef OMNI_VERSION\n#define OMNI_VERSION \"[^\"]*\"\n#endif\n",
        t,
    )
    if m:
        t = t[: m.end()] + EXTERN + t[m.end() :]
        print("injected storage externs after #endif OMNI_VERSION")
    else:
        # fallback: after first includes block
        m2 = re.search(r"#include <stdbool\.h>\n", t)
        if m2:
            t = t[: m2.end()] + EXTERN + t[m2.end() :]
            print("injected storage externs after stdbool")
        else:
            t = EXTERN + t
            print("injected storage externs at top")

soft = re.compile(
    r"static void sdmmc_probe\(void\)\{\s*battery_probe\(\);\s*g_sd_ok = \(g_battery >= 0\) \? 1 : 0;\s*\}",
    re.S,
)
real_probe = r'''static void sdmmc_probe(void){
        if(!sdmmc_card_inserted()){ g_sd_ok=0; return; }
        g_sd_ok = (sdmmc_init()==0) ? 1 : 0;
}'''
if soft.search(t):
    t = soft.sub(real_probe, t, count=1)
    print("replaced soft sdmmc_probe")

# Real R4/DSTT backup
old = '''clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                        draw_text(left_x(L("BACKUP OK","BACKUP OK"),12),100,L("BACKUP OK","BACKUP OK"),80,255,120);
                                        draw_text(left_x(bpath,12),120,bpath,180,200,220);'''
new = '''clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                        {unsigned char pay[64]; unsigned int lba=0; int br; int zi;
                                        pay[0]=(unsigned char)(is_r4?'R':'D');
                                        pay[1]=(unsigned char)(is_r4?'4':'S');
                                        pay[2]='B';pay[3]='K';
                                        for(zi=4;zi<64;zi++)pay[zi]=(unsigned char)zi;
                                        br=o10_backup_write(pay,64,&lba);
                                        if(br==0) draw_text(left_x(L("BACKUP OK","BACKUP OK"),12),100,L("BACKUP OK","BACKUP OK"),80,255,120);
                                        else draw_text(left_x(L("BACKUP FAIL","BACKUP FEHLER"),12),100,L("BACKUP FAIL","BACKUP FEHLER"),255,80,80);
                                        draw_text(left_x(bpath,12),120,bpath,180,200,220);}'''
if old in t:
    t = t.replace(old, new, 1)
    print("wired real o10_backup_write")

old_r = '''clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                        draw_text(left_x(L("RESTORE OK","RESTORE OK"),12),100,L("RESTORE OK","RESTORE OK"),80,255,120);'''
new_r = '''clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                        {unsigned char pay[64]; unsigned int ln=0; int rr=o10_backup_read(pay,64,&ln);
                                        if(rr==0) draw_text(left_x(L("RESTORE OK","RESTORE OK"),12),100,L("RESTORE OK","RESTORE OK"),80,255,120);
                                        else draw_text(left_x(L("RESTORE FAIL","RESTORE FEHLER"),12),100,L("RESTORE FAIL","RESTORE FEHLER"),255,80,80);}'''
if old_r in t:
    t = t.replace(old_r, new_r, 1)
    print("wired real o10_backup_read")

main.write_text(t)
print("patched", main, "bytes", len(t))
# sanity: externs must not sit only inside ifndef
if re.search(r"#ifndef OMNI_VERSION\n#define OMNI_VERSION[^\n]+\n/\* === real storage", t):
    print("ERROR: externs still inside ifndef")
    sys.exit(1)
print("extern placement OK")
