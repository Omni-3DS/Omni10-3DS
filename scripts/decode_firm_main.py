#!/usr/bin/env python3
"""Decode firm/arm9/main.c from scripts/firm_main_{0..3}.zlib.b64 and patch menu scroll."""
import base64
import pathlib
import re
import sys
import zlib

root = pathlib.Path(__file__).resolve().parents[1]
parts = []
for i in range(4):
    p = root / "scripts" / f"firm_main_{i}.zlib.b64"
    if not p.is_file():
        print("MISSING", p)
        sys.exit(1)
    parts.append(p.read_text().replace("\n", "").replace(" ", "").strip())

b64 = "".join(parts)
data = zlib.decompress(base64.b64decode(b64))
text = data.decode("utf-8")

# --- Patch: scrollable main menu (8 visible rows) ---
SCROLL_MENU = r'''static void screen_menu(void){
        int sel=0,scroll=0;
        const int vis=8;
        while(1){
                wifi_probe();battery_probe();g_ticks++;
                const char *items[]={L("ABOUT","INFO"),L("SYSTEM INFO","SYSTEMINFO"),L("HOME SCRIPTS","HOME SKRIPTE"),L("SETTINGS","EINSTELLUNGEN"),L("INTERNET / WIFI","INTERNET / WIFI"),L("BATTERY INFO","AKKU INFO"),L("BUTTON TEST","TASTEN TEST"),L("LED TEST","LED TEST"),L("FILE BROWSER","DATEIBROWSER"),L("FTP","FTP"),L("REBOOT","NEUSTART"),L("POWER OFF","AUSSCHALTEN")};
                const int n=12;
                if(sel<scroll)scroll=sel;
                if(sel>=scroll+vis)scroll=sel-vis+1;
                if(scroll<0)scroll=0;
                if(scroll>n-vis)scroll=(n>vis)?(n-vis):0;
                clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                draw_text(left_x(L("MAIN MENU","HAUPTMENUE"),12),34,L("MAIN MENU","HAUPTMENUE"),180,210,255);
                if(scroll>0)draw_text(left_x("^ more",12),48,"^ more",120,160,200);
                if(scroll+vis<n)draw_text(left_x("v more",12),48+vis*17+6,"v more",120,160,200);
                for(int vi=0;vi<vis;vi++){
                        int i=scroll+vi;if(i>=n)break;
                        int y=52+vi*17;
                        if(i==sel){fill_rect(4,y-2,SCREEN_W-8,15,0,70,110);draw_text(left_x(items[i],16),y,items[i],255,255,120);}
                        else draw_text(left_x(items[i],16),y,items[i],190,195,210);
                }
                draw_footer(L("A SELECT | X SCRIPTS | START+SELECT OFF","A | X SKRIPTE | START+SELECT AUS"));
                draw_bot_help(L("UP/DOWN scroll  X = scripts","HOCH/RUNTER scrollen  X = Skripte"),L("A select","A waehlen"));
                drain();
                uint32_t k=wait_key();
                if(k&BTN_X){screen_scripts_hub();continue;}
                if(k&BTN_UP){sel--;if(sel<0)sel=n-1;}
                if(k&BTN_DOWN){sel++;if(sel>=n)sel=0;}
                if(k&BTN_A){
                        switch(sel){
                        case 0:screen_about();break;
                        case 1:screen_sysinfo();break;
                        case 2:screen_scripts_hub();break;
                        case 3:screen_settings();break;
                        case 4:screen_internet();break;
                        case 5:screen_battery();break;
                        case 6:screen_buttons();break;
                        case 7:screen_led();break;
                        case 8:screen_filebrowser();break;
                        case 9:screen_ftp();break;
                        case 10:if(confirm(L("REBOOT?","NEUSTART?"),L("Restart the console","Konsole neu starten")))reboot();break;
                        case 11:if(confirm(L("POWER OFF?","AUSSCHALTEN?"),L("Turn console off","Konsole ausschalten")))power_off();break;
                        }
                }
        }
}
'''

m = re.search(r"static void screen_menu\(void\)\{.*?\n(?=int main)", text, re.S)
if m:
    text = text[: m.start()] + SCROLL_MENU + "\n" + text[m.end() :]
    print("patched screen_menu with scroll viewport")
else:
    print("WARN: screen_menu not found for scroll patch")

out = root / "firm" / "arm9" / "main.c"
out.write_text(text)
print(f"wrote {out} ({len(text)} bytes)")

required = (
    b"int main",
    b"screen_filebrowser",
    b"screen_ftp",
    b"screen_scripts_hub",
    b"HOME SCRIPTS",
)
ok = True
for tag in required:
    present = tag in text.encode()
    print(tag.decode(), "OK" if present else "MISSING")
    if not present:
        ok = False
if b"scroll" in text.encode() or b"vis=8" in text.encode():
    print("menu scroll OK")
else:
    print("menu scroll MISSING")
    ok = False

sys.exit(0 if ok else 1)
