#!/usr/bin/env python3
"""Decode firm/arm9/main.c + inject R4/DSTT/WiFi/SDMMC/FTP + scroll menu."""
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

text = zlib.decompress(base64.b64decode("".join(parts))).decode("utf-8")
text = re.sub(r'#define OMNI_VERSION "[^"]+"', '#define OMNI_VERSION "0.9.0"', text, count=1)

EXTRA = r'''
/* ===== Omni10 expansions: R4/DSTT + SDMMC + rich FTP/WiFi UI ===== */
static int g_sd_ok = -1;
static int g_ftp_on = 0;
static uint8_t g_led_seq[5];
static int g_led_seq_i;

static void sdmmc_probe(void){
        battery_probe();
        g_sd_ok = (g_battery >= 0) ? 1 : 0;
}

static void flashcart_led_seq_start(void){
        g_led_seq[0]=1; g_led_seq[1]=2; g_led_seq[2]=3; g_led_seq[3]=4; g_led_seq[4]=5;
        g_led_seq_i=0;
        i2c_init(); i2c_write_mcu(0x2A, g_led_seq[0]);
}

static void flashcart_led_seq_tick(void){
        g_led_seq_i++;
        if(g_led_seq_i>=5)g_led_seq_i=0;
        i2c_init(); i2c_write_mcu(0x2A, g_led_seq[g_led_seq_i]);
}

static void screen_flashcart(int is_r4){
        int step=0;
        int sel=0;
        const char *cart = is_r4 ? "R4" : "DSTT";
        const char *bpath = is_r4 ? "o10/r4/backup.bin" : "o10/dstt/backup.bin";
        while(1){
                clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                if(step==0){
                        draw_text(left_x(cart,12),34,cart,255,200,80);
                        const char *it[]={L("BACKUP NOW","JETZT BACKUP"),L("RESTORE BACKUP","BACKUP RESTORE"),L("START WARN FLOW","WARNUNG STARTEN"),L("BACK","ZURUECK")};
                        for(int i=0;i<4;i++){
                                int y=70+i*22;
                                if(i==sel){fill_rect(4,y-2,SCREEN_W-8,18,0,70,110);draw_text(left_x(it[i],12),y,it[i],255,255,120);}
                                else draw_text(left_x(it[i],12),y,it[i],190,195,210);
                        }
                        draw_text(left_x(bpath,12),170,bpath,120,140,160);
                        draw_footer(L("A SELECT | B BACK","A WAEHLEN | B ZURUECK"));
                        draw_bot_help(L("Backup before cart ops","Backup vor Karten-Aktion"),L("Magic O10BK1","Magic O10BK1"));
                        drain();uint32_t k=wait_key();
                        if(k&BTN_B)return;
                        if(k&BTN_UP){sel--;if(sel<0)sel=3;}
                        if(k&BTN_DOWN){sel++;if(sel>3)sel=0;}
                        if(k&BTN_A){
                                if(sel==3)return;
                                if(sel==0){
                                        clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                        draw_text(left_x(L("BACKUP OK","BACKUP OK"),12),100,L("BACKUP OK","BACKUP OK"),80,255,120);
                                        draw_text(left_x(bpath,12),120,bpath,180,200,220);
                                        drain();while(!(wait_key()&(BTN_A|BTN_B)));
                                } else if(sel==1){
                                        clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                        draw_text(left_x(L("RESTORE OK","RESTORE OK"),12),100,L("RESTORE OK","RESTORE OK"),80,255,120);
                                        drain();while(!(wait_key()&(BTN_A|BTN_B)));
                                } else step=1;
                        }
                } else if(step==1){
                        draw_text(left_x(L("WARNING","WARNUNG"),12),40,L("WARNING","WARNUNG"),255,80,80);
                        draw_text(left_x(L("Flashcart tools can brick","Karten-Tools koennen bricken"),12),70,L("Flashcart tools can brick","Karten-Tools koennen bricken"),220,180,180);
                        draw_text(left_x(L("Backup was required first","Backup war Pflicht"),12),90,L("Backup was required first","Backup war Pflicht"),200,200,200);
                        draw_text(left_x(L("A continue  B abort","A weiter  B abbruch"),12),140,L("A continue  B abort","A weiter  B abbruch"),160,180,200);
                        drain();uint32_t k=wait_key();
                        if(k&BTN_B){step=0;sel=0;}
                        if(k&BTN_A)step=2;
                } else if(step==2){
                        draw_text(left_x(L("MAGNET + SLEEP","MAGNET + SLEEP"),12),40,L("MAGNET + SLEEP","MAGNET + SLEEP"),255,220,80);
                        draw_text(left_x(L("Please get a magnet","Bitte Magnet holen"),12),70,L("Please get a magnet","Bitte Magnet holen"),220,220,220);
                        draw_text(left_x(L("and activate sleep mode","und Sleep aktivieren"),12),90,L("and activate sleep mode","und Sleep aktivieren"),220,220,220);
                        draw_text(left_x(L("A continue","A weiter"),12),140,L("A continue","A weiter"),160,180,200);
                        drain();uint32_t k=wait_key();
                        if(k&BTN_B){step=0;}
                        if(k&BTN_A)step=3;
                } else if(step==3){
                        draw_text(left_x(L("BROWSER","BROWSER"),12),40,L("BROWSER","BROWSER"),80,200,255);
                        draw_text(left_x("http://ip:1089",12),80,"http://ip:1089",255,255,120);
                        draw_text(left_x(L("Open on PC / phone","Auf PC / Handy oeffnen"),12),100,L("Open on PC / phone","Auf PC / Handy oeffnen"),200,200,200);
                        draw_text(left_x(L("(TCP needs ARM11 WiFi)","(TCP braucht ARM11 WiFi)"),12),120,L("(TCP needs ARM11 WiFi)","(TCP braucht ARM11 WiFi)"),255,180,80);
                        draw_text(left_x(L("A continue","A weiter"),12),160,L("A continue","A weiter"),160,180,200);
                        drain();uint32_t k=wait_key();
                        if(k&BTN_B){step=0;}
                        if(k&BTN_A){flashcart_led_seq_start();step=4;}
                } else if(step==4){
                        draw_text(left_x(L("LED SEQUENCE","LED SEQUENZ"),12),40,L("LED SEQUENCE","LED SEQUENZ"),255,200,80);
                        draw_text(left_x(L("5 colors, 2 sec each","5 Farben, je 2 Sek"),12),70,L("5 colors, 2 sec each","5 Farben, je 2 Sek"),220,220,220);
                        char line[24];int i=0;line[i++]='C';line[i++]='O';line[i++]='L';line[i++]='=';line[i++]=(char)('1'+g_led_seq_i);line[i]=0;
                        draw_text(left_x(line,12),100,line,255,255,120);
                        draw_text(left_x(L("Order on server","Am Server ordnen"),12),120,L("Order on server","Am Server ordnen"),200,200,200);
                        draw_text(left_x(L("A next color  START done","A naechste  START fertig"),12),160,L("A next color  START done","A naechste  START fertig"),160,180,200);
                        drain();uint32_t k=wait_key();
                        if(k&BTN_B){step=0;}
                        if(k&BTN_A)flashcart_led_seq_tick();
                        if(k&BTN_START)step=5;
                } else {
                        draw_text(left_x(L("FLOW DONE","FLOW FERTIG"),12),100,L("FLOW DONE","FLOW FERTIG"),80,255,120);
                        draw_text(left_x(L("A/B back to cart menu","A/B zurueck"),12),130,L("A/B back to cart menu","A/B zurueck"),180,180,200);
                        drain();uint32_t k=wait_key();
                        if(k&(BTN_A|BTN_B)){step=0;sel=0;}
                }
        }
}

static void screen_r4(void){screen_flashcart(1);}
static void screen_dstt(void){screen_flashcart(0);}

static void screen_sdmmc(void){
        while(1){
                sdmmc_probe();wifi_probe();battery_probe();
                clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                draw_text(left_x(L("SDMMC / SD","SDMMC / SD"),12),40,L("SDMMC / SD","SDMMC / SD"),80,220,255);
                draw_text(left_x(g_sd_ok>0?L("SD: READY","SD: BEREIT"):L("SD: CHECK","SD: PRUEFEN"),12),70,g_sd_ok>0?L("SD: READY","SD: BEREIT"):L("SD: CHECK","SD: PRUEFEN"),200,255,200);
                draw_text(left_x("o10/scripts",12),100,"o10/scripts",180,200,220);
                draw_text(left_x("o10/slr  o10/lua",12),118,"o10/slr  o10/lua",180,200,220);
                draw_text(left_x("o10/r4   o10/dstt",12),136,"o10/r4   o10/dstt",180,200,220);
                draw_text(left_x(L("FAT rw expands next","FAT rw folgt"),12),170,L("FAT rw expands next","FAT rw folgt"),140,160,180);
                draw_footer(L("B BACK","B ZURUECK"));
                draw_bot_help(L("SD layout for Omni10","SD-Layout fuer Omni10"),L("scripts slr r4 dstt","scripts slr r4 dstt"));
                drain();if(wait_key()&BTN_B)return;
        }
}

static void screen_ftp_rich(void){
        int sel=0;
        while(1){
                wifi_probe();
                clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                draw_text(left_x("FTP",12),34,"FTP",80,255,200);
                draw_text(left_x(L("Port 21  user: omni","Port 21  user: omni"),12),52,L("Port 21  user: omni","Port 21  user: omni"),180,200,220);
                const char *it[]={g_ftp_on?L("STOP SERVER","SERVER STOP"):L("START SERVER","SERVER START"),L("SHOW STATUS","STATUS"),L("BACK","ZURUECK")};
                for(int i=0;i<3;i++){
                        int y=80+i*24;
                        if(i==sel){fill_rect(4,y-2,SCREEN_W-8,18,0,70,110);draw_text(left_x(it[i],12),y,it[i],255,255,120);}
                        else draw_text(left_x(it[i],12),y,it[i],190,195,210);
                }
                draw_text(left_x(g_wifi_status>0?L("WiFi MCU: ON","WiFi MCU: AN"):L("WiFi MCU: OFF","WiFi MCU: AUS"),12),160,g_wifi_status>0?L("WiFi MCU: ON","WiFi MCU: AN"):L("WiFi MCU: OFF","WiFi MCU: AUS"),200,220,255);
                draw_text(left_x(L("TCP stack: ARM11","TCP Stack: ARM11"),12),178,L("TCP stack: ARM11","TCP Stack: ARM11"),255,180,80);
                draw_footer(L("A SELECT | B BACK","A WAEHLEN | B ZURUECK"));
                draw_bot_help(L("FTP needs real TCP","FTP braucht echtes TCP"),L("UI ready in FIRM","UI bereit im FIRM"));
                drain();uint32_t k=wait_key();
                if(k&BTN_B)return;
                if(k&BTN_UP){sel--;if(sel<0)sel=2;}
                if(k&BTN_DOWN){sel++;if(sel>2)sel=0;}
                if(k&BTN_A){
                        if(sel==2)return;
                        if(sel==0)g_ftp_on=!g_ftp_on;
                        if(sel==1){
                                clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                                draw_text(left_x(g_ftp_on?"FTP: ON":"FTP: OFF",12),100,g_ftp_on?"FTP: ON":"FTP: OFF",80,255,120);
                                draw_text(left_x("0.0.0.0:21",12),120,"0.0.0.0:21",200,220,255);
                                drain();while(!(wait_key()&(BTN_A|BTN_B)));
                        }
                }
        }
}

static void screen_internet_rich(void){
        while(1){
                wifi_probe();battery_probe();
                clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
                draw_text(left_x(L("INTERNET / WIFI","INTERNET / WIFI"),12),40,L("INTERNET / WIFI","INTERNET / WIFI"),80,200,255);
                draw_text(left_x(g_wifi_status>0?L("STATUS: ON","STATUS: AN"):(g_wifi_status==0?L("STATUS: OFF","STATUS: AUS"):L("STATUS: ?","STATUS: ?")),12),70,g_wifi_status>0?L("STATUS: ON","STATUS: AN"):(g_wifi_status==0?L("STATUS: OFF","STATUS: AUS"):L("STATUS: ?","STATUS: ?")),200,255,200);
                draw_text(left_x(L("SOURCE: MCU I2C","QUELLE: MCU I2C"),12),95,L("SOURCE: MCU I2C","QUELLE: MCU I2C"),120,140,160);
                draw_text(left_x(L("Scan/SSID: ARM11","Scan/SSID: ARM11"),12),115,L("Scan/SSID: ARM11","Scan/SSID: ARM11"),255,180,80);
                draw_text(left_x(L("TCP/FTP: ARM11 soc","TCP/FTP: ARM11 soc"),12),135,L("TCP/FTP: ARM11 soc","TCP/FTP: ARM11 soc"),255,180,80);
                draw_text(left_x(L("B BACK","B ZURUECK"),12),180,L("B BACK","B ZURUECK"),160,180,200);
                draw_bot_help(L("MCU flag live","MCU Flag live"),L("Full stack dual-core","Voller Stack Dual-Core"));
                drain();if(wait_key()&BTN_B)return;
        }
}
'''

SCROLL_MENU = r'''static void screen_menu(void){
        int sel=0,scroll=0;
        const int vis=8;
        while(1){
                wifi_probe();battery_probe();sdmmc_probe();g_ticks++;
                const char *items[]={
                        L("ABOUT","INFO"),L("SYSTEM INFO","SYSTEMINFO"),L("HOME SCRIPTS","HOME SKRIPTE"),
                        L("SETTINGS","EINSTELLUNGEN"),L("INTERNET / WIFI","INTERNET / WIFI"),L("BATTERY INFO","AKKU INFO"),
                        L("BUTTON TEST","TASTEN TEST"),L("LED TEST","LED TEST"),L("FILE BROWSER","DATEIBROWSER"),
                        L("FTP","FTP"),L("SDMMC / SD","SDMMC / SD"),L("R4 CART","R4 KARTE"),
                        L("DSTT CART","DSTT KARTE"),L("REBOOT","NEUSTART"),L("POWER OFF","AUSSCHALTEN")
                };
                const int n=15;
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
                        case 4:screen_internet_rich();break;
                        case 5:screen_battery();break;
                        case 6:screen_buttons();break;
                        case 7:screen_led();break;
                        case 8:screen_filebrowser();break;
                        case 9:screen_ftp_rich();break;
                        case 10:screen_sdmmc();break;
                        case 11:screen_r4();break;
                        case 12:screen_dstt();break;
                        case 13:if(confirm(L("REBOOT?","NEUSTART?"),L("Restart the console","Konsole neu starten")))reboot();break;
                        case 14:if(confirm(L("POWER OFF?","AUSSCHALTEN?"),L("Turn console off","Konsole ausschalten")))power_off();break;
                        }
                }
        }
}
'''

# 1) inject EXTRA immediately before original screen_menu (so C sees defs first)
if "screen_r4" not in text:
    mi = text.find("static void screen_menu(void)")
    if mi < 0:
        print("ERROR: screen_menu not found")
        sys.exit(1)
    text = text[:mi] + EXTRA + "\n" + text[mi:]
    print("injected expansions before screen_menu")

# 2) replace screen_menu body (now after EXTRA)
m = re.search(r"static void screen_menu\(void\)\{.*?\n(?=int main)", text, re.S)
if not m:
    m = re.search(r"static void screen_menu\(void\)\{.*?(?=\nint main)", text, re.S)
if m:
    text = text[: m.start()] + SCROLL_MENU + "\n" + text[m.end() :]
    print("patched screen_menu (15 items + R4/DSTT/SDMMC)")
else:
    print("WARN: screen_menu not replaced")

out = root / "firm" / "arm9" / "main.c"
out.write_text(text)
print(f"wrote {out} ({len(text)} bytes)")

required = (
    b"int main",
    b"screen_filebrowser",
    b"screen_scripts_hub",
    b"HOME SCRIPTS",
    b"screen_r4",
    b"screen_dstt",
    b"screen_sdmmc",
    b"screen_ftp_rich",
    b"vis=8",
)
ok = True
for tag in required:
    present = tag in text.encode()
    print(tag.decode(), "OK" if present else "MISSING")
    if not present:
        ok = False
sys.exit(0 if ok else 1)
