/* version from Makefile -DOMNI_VERSION if set */
#ifndef OMNI_VERSION
#define OMNI_VERSION "0.8.2"
#endif
/* Omni10-3DS v0.8.2 - X-flip orient + left + net */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#define REG_PAD_HID (*(volatile uint32_t *)0x10146000)
#define PDN_MPCORE_CFG (*(volatile uint8_t *)0x10140FFC)
#define SCREEN_W 400
#define SCREEN_H 240
#define BOT_W 320
#define FB_TOP0 ((volatile uint8_t *)0x18300000)
#define FB_TOP1 ((volatile uint8_t *)0x18400000)
#define FB_BOT0 ((volatile uint8_t *)0x18346500)
#define FB_BOT1 ((volatile uint8_t *)0x18446500)
#define BTN_A (1u<<0)
#define BTN_B (1u<<1)
#define BTN_SELECT (1u<<2)
#define BTN_START (1u<<3)
#define BTN_RIGHT (1u<<4)
#define BTN_LEFT (1u<<5)
#define BTN_UP (1u<<6)
#define BTN_DOWN (1u<<7)
#define BTN_R (1u<<8)
#define BTN_L (1u<<9)
#define BTN_X (1u<<10)
#define BTN_Y (1u<<11)
#define PIXEL_OFFSET(x,y) (((SCREEN_W-1-(x))*SCREEN_H)+(SCREEN_H-1-(y)))
#define PIXEL_OFFSET_BOT(x,y) (((BOT_W-1-(x))*SCREEN_H)+(SCREEN_H-1-(y)))
#define I2C2_REGS_BASE 0x10144000
#define I2C_STOP (1u<<0)
#define I2C_START (1u<<1)
#define I2C_PAUSE (1u<<2)
#define I2C_ACK (1u<<4)
#define I2C_DIRE_READ (1u<<5)
#define I2C_IRQ_ENABLE (1u<<6)
#define I2C_ENABLE (1u<<7)
#define I2C_GET_ACK(r) (((r)>>4)&1u)
#define MCU_DEV_ADDR 0x4A
#define COL_BG_R 12
#define COL_BG_G 12
#define COL_BG_B 28
#define COL_HDR_R 16
#define COL_HDR_G 20
#define COL_HDR_B 48
typedef struct{volatile uint8_t REG_I2C_DATA;volatile uint8_t REG_I2C_CNT;volatile uint16_t REG_I2C_CNTEX;volatile uint16_t REG_I2C_SCL;}I2cRegs;
struct fb{uint8_t *top_left;uint8_t *top_right;uint8_t *bottom;};
static volatile uint8_t *g_top[2];static volatile uint8_t *g_bot[2];static int g_n_top,g_n_bot;
enum{LANG_EN=0,LANG_DE=1};static int g_lang=LANG_EN;static int g_wifi_status=-1;static int g_battery=-1;static int g_charging=0;static uint32_t g_ticks=0;
static const char *L(const char *en,const char *de){return g_lang==LANG_DE?de:en;}
void *memset(void *d,int v,size_t n){uint8_t *p=(uint8_t*)d;while(n--)*p++=(uint8_t)v;return d;}
size_t strlen(const char *s){size_t n=0;while(s[n])n++;return n;}
static int strncmp_s(const char *a,const char *b,int n){for(int i=0;i<n;i++){if(a[i]!=b[i])return (unsigned char)a[i]-(unsigned char)b[i];if(!a[i])return 0;}return 0;}
static void i2c_wait(I2cRegs *r){while(r->REG_I2C_CNT&I2C_ENABLE);}
static void i2c_init(void){I2cRegs *r=(I2cRegs*)I2C2_REGS_BASE;i2c_wait(r);r->REG_I2C_CNTEX=2;r->REG_I2C_SCL=1280;}
static bool i2c_write_mcu(uint8_t reg,uint8_t data){I2cRegs *r=(I2cRegs*)I2C2_REGS_BASE;for(int t=0;t<8;t++){i2c_wait(r);r->REG_I2C_DATA=MCU_DEV_ADDR;r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_START;i2c_wait(r);if(!I2C_GET_ACK(r->REG_I2C_CNT)){r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_PAUSE|I2C_STOP;continue;}r->REG_I2C_DATA=reg;r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE;i2c_wait(r);if(!I2C_GET_ACK(r->REG_I2C_CNT)){r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_PAUSE|I2C_STOP;continue;}r->REG_I2C_DATA=data;r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_STOP;i2c_wait(r);return true;}return false;}
static bool i2c_read_mcu(uint8_t reg,uint8_t *out){I2cRegs *r=(I2cRegs*)I2C2_REGS_BASE;for(int t=0;t<8;t++){i2c_wait(r);r->REG_I2C_DATA=MCU_DEV_ADDR;r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_START;i2c_wait(r);if(!I2C_GET_ACK(r->REG_I2C_CNT)){r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_PAUSE|I2C_STOP;continue;}r->REG_I2C_DATA=reg;r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE;i2c_wait(r);if(!I2C_GET_ACK(r->REG_I2C_CNT)){r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_PAUSE|I2C_STOP;continue;}r->REG_I2C_DATA=(uint8_t)(MCU_DEV_ADDR|1);r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_START;i2c_wait(r);if(!I2C_GET_ACK(r->REG_I2C_CNT)){r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_PAUSE|I2C_STOP;continue;}r->REG_I2C_CNT=I2C_ENABLE|I2C_IRQ_ENABLE|I2C_DIRE_READ|I2C_STOP;i2c_wait(r);*out=r->REG_I2C_DATA;return true;}return false;}
static void power_off(void){i2c_init();i2c_write_mcu(0x22,1u<<0);for(volatile int i=0;i<300000;i++)__asm__ volatile("nop");i2c_write_mcu(0x20,1u<<0);while(1);}
static void reboot(void){i2c_init();i2c_write_mcu(0x22,1u<<0);for(volatile int i=0;i<300000;i++)__asm__ volatile("nop");i2c_write_mcu(0x20,1u<<2);while(1);}
static int wifi_probe(void){uint8_t led=0,flags=0;int ok_led=i2c_read_mcu(0x2A,&led);int ok_flg=i2c_read_mcu(0x61,&flags);if(ok_flg&&(flags&1u)){i2c_write_mcu(0x61,(uint8_t)(flags&~1u));i2c_write_mcu(0x2A,0xF);ok_flg=i2c_read_mcu(0x61,&flags);ok_led=i2c_read_mcu(0x2A,&led);}if(!ok_led&&!ok_flg){g_wifi_status=-1;return -1;}if(ok_flg&&(flags&1u)){g_wifi_status=0;return 0;}if(ok_led&&led!=0){g_wifi_status=1;return 1;}g_wifi_status=0;return 0;}
static void battery_probe(void){uint8_t pct=0,st=0;if(i2c_read_mcu(0x0B,&pct)){g_battery=(int)pct;if(g_battery>100)g_battery=100;}else g_battery=-1;if(i2c_read_mcu(0x0F,&st))g_charging=(st&0x10)?1:0;else g_charging=0;}
static void drain(void){uint32_t z=0;__asm__ volatile("mcr p15,0,%0,c7,c10,4"::"r"(z):"memory");}
static void delay(int n){for(volatile int i=0;i<n;i++)__asm__ volatile("nop");}
static const uint8_t font8[64][8]={{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},{0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},{0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00},{0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},{0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00},{0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00},{0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00},{0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00},{0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00},{0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00},{0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},{0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00},{0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06},{0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00},{0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00},{0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00},{0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00},{0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00},{0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00},{0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00},{0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00},{0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00},{0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00},{0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00},{0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00},{0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00},{0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06},{0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00},{0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00},{0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00},{0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00},{0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00},{0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00},{0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00},{0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00},{0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00},{0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00},{0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00},{0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00},{0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00},{0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},{0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00},{0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00},{0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00},{0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},{0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00},{0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00},{0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00},{0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00},{0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00},{0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00},{0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},{0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00},{0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00},{0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},{0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00},{0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00},{0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00},{0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00},{0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00},{0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00},{0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}};
static void put(int x,int y,uint8_t r,uint8_t g,uint8_t b){if(x<0||y<0||x>=SCREEN_W||y>=SCREEN_H)return;int idx=PIXEL_OFFSET(x,y)*3;for(int i=0;i<g_n_top;i++){if(!g_top[i])continue;g_top[i][idx]=b;g_top[i][idx+1]=g;g_top[i][idx+2]=r;}}
static void put_bot(int x,int y,uint8_t r,uint8_t g,uint8_t b){if(x<0||y<0||x>=BOT_W||y>=SCREEN_H)return;int idx=PIXEL_OFFSET_BOT(x,y)*3;for(int i=0;i<g_n_bot;i++){if(!g_bot[i])continue;g_bot[i][idx]=b;g_bot[i][idx+1]=g;g_bot[i][idx+2]=r;}}
static void fill_rect(int x,int y,int w,int h,uint8_t r,uint8_t g,uint8_t b){for(int j=0;j<h;j++)for(int i=0;i<w;i++)put(x+i,y+j,r,g,b);}
static void clear_top(uint8_t r,uint8_t g,uint8_t b){fill_rect(0,0,SCREEN_W,SCREEN_H,r,g,b);}
static void clear_bot(uint8_t r,uint8_t g,uint8_t b){for(int y=0;y<SCREEN_H;y++)for(int x=0;x<BOT_W;x++)put_bot(x,y,r,g,b);}
static void draw_char(int x,int y,char c,uint8_t r,uint8_t g,uint8_t b){unsigned char uc=(unsigned char)c;if(uc>='a'&&uc<='z')uc=(unsigned char)(uc-32);int idx=(int)uc-0x20;if(idx<0||idx>=64)idx=0;const uint8_t *glyph=font8[idx];for(int row=0;row<8;row++){uint8_t bits=glyph[row];for(int col=0;col<8;col++)if(bits&(0x80>>col))put(x+col,y+row,r,g,b);}}
static void draw_char_bot(int x,int y,char c,uint8_t r,uint8_t g,uint8_t b){unsigned char uc=(unsigned char)c;if(uc>='a'&&uc<='z')uc=(unsigned char)(uc-32);int idx=(int)uc-0x20;if(idx<0||idx>=64)idx=0;const uint8_t *glyph=font8[idx];for(int row=0;row<8;row++){uint8_t bits=glyph[row];for(int col=0;col<8;col++)if(bits&(0x80>>col))put_bot(x+col,y+row,r,g,b);}}
static void draw_text(int x,int y,const char *s,uint8_t r,uint8_t g,uint8_t b){int n=(int)strlen(s);for(int i=0;i<n;i++)draw_char(x+i*8,y,s[n-1-i],r,g,b);}
static void draw_text_bot(int x,int y,const char *s,uint8_t r,uint8_t g,uint8_t b){int n=(int)strlen(s);for(int i=0;i<n;i++)draw_char_bot(x+i*8,y,s[n-1-i],r,g,b);}
static void draw_text_centered(int y,const char *s,uint8_t r,uint8_t g,uint8_t b){int w=(int)strlen(s)*8;draw_text((SCREEN_W-w)/2,y,s,r,g,b);}
static void draw_text_bot_centered(int y,const char *s,uint8_t r,uint8_t g,uint8_t b){int w=(int)strlen(s)*8;draw_text_bot((BOT_W-w)/2,y,s,r,g,b);}
static uint32_t pad_raw(void){return ~REG_PAD_HID;}
static uint32_t wait_key(void){uint32_t prev=pad_raw();delay(35000);while(1){uint32_t k=pad_raw();uint32_t pressed=k&~prev;if(pressed)return pressed;prev=k;if((k&BTN_START)&&(k&BTN_SELECT))power_off();delay(1200);}}
static int is_new3ds(void){return (PDN_MPCORE_CFG&2)!=0;}
static int left_x(const char *s,int margin){int n=(int)strlen(s);if(n<1)n=1;int x=SCREEN_W-1-margin-(n-1)*8;return x<0?0:x;}
static int left_x_bot(const char *s,int margin){int n=(int)strlen(s);if(n<1)n=1;int x=BOT_W-1-margin-(n-1)*8;return x<0?0:x;}
static void draw_status_bar(void){{const char *label;uint8_t r,g,b;if(g_wifi_status<0){label="NET ?";r=180;g=180;b=100;}else if(g_wifi_status>0){label="NET ON";r=40;g=255;b=120;}else{label="NET OFF";r=255;g=80;b=80;}int w=(int)strlen(label)*8+8;fill_rect(8,6,w,16,8,12,32);draw_text(10,10,label,r,g,b);}{char buf[14];uint8_t r,g,b;int i=0;if(g_battery<0){buf[0]='B';buf[1]='A';buf[2]='T';buf[3]='?';buf[4]=0;r=180;g=180;b=100;}else{int p=g_battery;buf[i++]='B';buf[i++]='A';buf[i++]='T';if(g_charging)buf[i++]='+';buf[i++]=' ';if(p>=100){buf[i++]='1';buf[i++]='0';buf[i++]='0';}else if(p>=10){buf[i++]=(char)('0'+p/10);buf[i++]=(char)('0'+p%10);}else buf[i++]=(char)('0'+p);buf[i++]='%';buf[i]=0;if(p>50){r=40;g=255;b=120;}else if(p>20){r=255;g=200;b=80;}else{r=255;g=80;b=80;}if(g_charging){r=80;g=180;b=255;}}int w=(int)strlen(buf)*8+8;int x=SCREEN_W-w-8;if(x<120)x=120;fill_rect(x,6,w,16,8,12,32);draw_text(x+4,10,buf,r,g,b);}}
static void draw_header(void){fill_rect(0,0,SCREEN_W,28,COL_HDR_R,COL_HDR_G,COL_HDR_B);draw_text(left_x("OMNI10",12),10,"OMNI10",0,220,255);draw_status_bar();fill_rect(0,28,SCREEN_W,2,0,180,220);}
static void draw_footer(const char *hint){fill_rect(0,SCREEN_H-22,SCREEN_W,22,COL_HDR_R,COL_HDR_G,COL_HDR_B);draw_text(left_x(hint,12),SCREEN_H-15,hint,180,190,210);}
static void draw_bot_help(const char *l1,const char *l2){clear_bot(COL_BG_R,COL_BG_G,COL_BG_B);draw_text_bot(left_x_bot("OMNI10",12),40,"OMNI10",0,180,220);if(l1)draw_text_bot(left_x_bot(l1,12),100,l1,200,200,210);if(l2)draw_text_bot(left_x_bot(l2,12),120,l2,140,150,170);{const char *off=L("START+SELECT = OFF","START+SELECT = AUS");draw_text_bot(left_x_bot(off,12),200,off,120,130,150);}}
static int confirm(const char *title,const char *msg){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(title,12),70,title,255,200,80);draw_text(left_x(msg,12),110,msg,200,200,210);draw_text(left_x(L("A = YES   B = NO","A = JA   B = NEIN"),12),150,L("A = YES   B = NO","A = JA   B = NEIN"),160,180,200);draw_footer(L("CONFIRM","BESTAETIGEN"));draw_bot_help(L("Confirm action","Aktion bestaetigen"),NULL);drain();while(1){uint32_t k=wait_key();if(k&BTN_A)return 1;if(k&BTN_B)return 0;}}
static void screen_about(void){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x("OMNI10-3DS",12),50,"OMNI10-3DS",255,255,255);draw_text(left_x(L("CUSTOM FIRM PAYLOAD","EIGENES FIRM PAYLOAD"),12),72,L("CUSTOM FIRM PAYLOAD","EIGENES FIRM PAYLOAD"),140,200,230);draw_text(left_x("VERSION " OMNI_VERSION,12),100,"VERSION " OMNI_VERSION,160,160,180);draw_text(left_x(".O10  |  LUA  |  FTP",12),122,".O10  |  LUA  |  FTP",100,180,210);draw_text(left_x("GITHUB.COM/OMNI-3DS",12),144,"GITHUB.COM/OMNI-3DS",90,140,180);draw_text(left_x(L("FULL ACCESS. NO LIMITS.","VOLLER ZUGRIFF. KEINE LIMITS."),12),176,L("FULL ACCESS. NO LIMITS.","VOLLER ZUGRIFF. KEINE LIMITS."),0,200,180);draw_footer(L("B = BACK","B = ZURUECK"));draw_bot_help(L("About Omni10","Ueber Omni10"),L("B to go back","B = zurueck"));drain();while(1){if(wait_key()&BTN_B)return;}}
static void screen_sysinfo(void){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("SYSTEM INFO","SYSTEMINFO"),12),40,L("SYSTEM INFO","SYSTEMINFO"),200,220,255);draw_text(left_x(L("CONSOLE:","KONSOLE:"),12),70,L("CONSOLE:","KONSOLE:"),160,160,180);draw_text(140,70,is_new3ds()?"NEW 3DS FAMILY":"OLD 3DS FAMILY",0,255,180);draw_text(left_x("ENTRY:",12),90,"ENTRY:",160,160,180);draw_text(140,90,"0x08000040",200,200,220);draw_text(left_x("FB:",12),110,"FB:",160,160,180);draw_text(140,110,"RGB888",200,200,220);draw_text(left_x("LANG:",12),130,"LANG:",160,160,180);draw_text(140,130,g_lang==LANG_DE?"DE":"EN",200,200,220);draw_text(left_x("STATUS:",12),150,"STATUS:",160,160,180);draw_text(140,150,L("RUNNING","LAEUFT"),0,255,120);draw_text(left_x("WIFI:",12),170,"WIFI:",160,160,180);if(g_wifi_status>0)draw_text(140,170,"ON",40,255,120);else if(g_wifi_status==0)draw_text(140,170,"OFF",255,80,80);else draw_text(140,170,"?",180,180,100);draw_footer(L("B = BACK","B = ZURUECK"));draw_bot_help(L("Hardware details","Hardwaredetails"),NULL);drain();while(1){if(wait_key()&BTN_B)return;}}
static void screen_internet(void){while(1){wifi_probe();clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("INTERNET TEST","INTERNET TEST"),12),40,L("INTERNET TEST","INTERNET TEST"),200,220,255);draw_text(left_x(L("STATUS:","STATUS:"),12),70,L("STATUS:","STATUS:"),160,160,180);if(g_wifi_status>0)draw_text(140,70,L("CONNECTED / LED ON","VERBUNDEN / LED AN"),40,255,120);else if(g_wifi_status==0)draw_text(140,70,L("OFF / DISABLED","AUS / DEAKTIVIERT"),255,80,80);else draw_text(140,70,L("UNKNOWN","UNBEKANNT"),180,180,100);draw_text(left_x(L("SOURCE: MCU I2C","QUELLE: MCU I2C"),12),100,L("SOURCE: MCU I2C","QUELLE: MCU I2C"),120,140,160);draw_text(left_x("REG 0x2A WIFI LED",12),120,"REG 0x2A WIFI LED",120,140,160);draw_text(left_x("REG 0x61 WIRELESS FLAG",12),140,"REG 0x61 WIRELESS FLAG",120,140,160);draw_text(left_x(L("A = REFRESH","A = AKTUALISIEREN"),12),175,L("A = REFRESH","A = AKTUALISIEREN"),160,180,200);draw_text(left_x(L("B = BACK","B = ZURUECK"),12),195,L("B = BACK","B = ZURUECK"),160,180,200);draw_footer(L("A REFRESH  |  B BACK","A AKTUALISIEREN  |  B ZURUECK"));draw_bot_help(L("WiFi connection test","WLAN Verbindungstest"),L("Reads MCU WiFi state","Liest MCU WLAN Status"));drain();uint32_t k=wait_key();if(k&BTN_B)return;}}
static void screen_settings(void){int sel=0;while(1){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("SETTINGS","EINSTELLUNGEN"),12),40,L("SETTINGS","EINSTELLUNGEN"),200,220,255);const char *items[2];items[0]=g_lang==LANG_DE?"SPRACHE: DEUTSCH":"LANGUAGE: ENGLISH";items[1]=L("BACK","ZURUECK");for(int i=0;i<2;i++){int y=90+i*28;if(i==sel){fill_rect(8,y-4,SCREEN_W-16,18,0,70,110);draw_text(left_x(items[i],16),y,items[i],255,255,120);}else draw_text(left_x(items[i],16),y,items[i],190,195,210);}draw_footer(L("A SELECT  |  B BACK","A WAEHLEN  |  B ZURUECK"));draw_bot_help(L("Change language","Sprache aendern"),L("A toggle  B back","A umschalten  B zurueck"));drain();uint32_t k=wait_key();if(k&BTN_B)return;if((k&BTN_UP)||(k&BTN_DOWN))sel=1-sel;if(k&BTN_A){if(sel==0)g_lang=1-g_lang;else return;}}}

/* o10 / SLR / Lua script hub (X = quick HOME scripts) */
static const char *demo_hello="PRINT HELLO FROM O10\nWAIT\nPRINT OMNI10 RULES\nWAIT\nPRINT DONE\n";
static const char *demo_info="PRINT SYSTEM CHECK\nWAIT\nPRINT PAYLOAD OK\nWAIT\nPRINT END\n";
static const char *slr_hello="print \"Hello SLR\"\nwait 300\ninfo\nprint \"Done\"\n";
static void o10_run(const char *src){char line[48];int li=0,log_y=60;clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("O10 RUN","O10 RUN"),12),40,L("O10 RUN","O10 RUN"),0,220,255);draw_bot_help(L("Running .o10"," .o10 laeuft"),L("B abort","B abbruch"));drain();while(*src){if(pad_raw()&BTN_B)break;if(*src=='\n'||*src==0){line[li]=0;if(li>0){if(strncmp_s(line,"PRINT ",6)==0){draw_text(left_x(line+6,12),log_y,line+6,200,255,200);log_y+=14;if(log_y>200)log_y=60;drain();}else if(strncmp_s(line,"WAIT",4)==0)delay(600000);else if(strncmp_s(line,"POWEROFF",8)==0)power_off();else if(strncmp_s(line,"REBOOT",6)==0)reboot();}li=0;if(*src==0)break;src++;continue;}if(li<46)line[li++]=*src;src++;}draw_text(left_x(L("A/B BACK","A/B ZURUECK"),12),210,L("A/B BACK","A/B ZURUECK"),160,160,180);drain();while(1){uint32_t k=wait_key();if(k&(BTN_A|BTN_B))return;}}
static void screen_lua_stub(void){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x("LUA",12),50,"LUA",200,220,255);draw_text(left_x(L("NOT EMBEDDED YET","NOCH NICHT DRIN"),12),90,L("NOT EMBEDDED YET","NOCH NICHT DRIN"),255,180,80);draw_text(left_x("sdmc:/o10/lua/",12),120,"sdmc:/o10/lua/",140,150,170);draw_text(left_x(L("B = BACK","B = ZURUECK"),12),160,L("B = BACK","B = ZURUECK"),160,180,200);draw_footer(L("B BACK","B ZURUECK"));draw_bot_help(L("Lua planned","Lua geplant"),NULL);drain();while(1){if(wait_key()&BTN_B)return;}}
static void screen_slr_run(void){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x("SLR",12),40,"SLR",200,220,255);draw_text(left_x(L("ON-DEVICE COMPILE","COMPILE AUF 3DS"),12),70,L("ON-DEVICE COMPILE","COMPILE AUF 3DS"),0,255,180);draw_text(left_x("sdmc:/o10/slr/",12),100,"sdmc:/o10/slr/",140,150,170);draw_text(left_x(L("A = DEMO SLR->O10","A = DEMO SLR->O10"),12),130,L("A = DEMO SLR->O10","A = DEMO SLR->O10"),160,180,200);draw_footer(L("A RUN | B BACK","A START | B ZURUECK"));draw_bot_help(L("SLR on console","SLR auf Konsole"),NULL);drain();while(1){uint32_t k=wait_key();if(k&BTN_B)return;if(k&BTN_A){/* treat SLR demo as text o10-like lines via print conversion */o10_run("PRINT Hello SLR\nWAIT\nPRINT Done\n");return;}}}
static void screen_scripts_hub(void){int sel=0;while(1){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("HOME SCRIPTS","HOME SKRIPTE"),12),34,L("HOME SCRIPTS","HOME SKRIPTE"),200,220,255);draw_text(left_x("sdmc:/o10/",12),48,"sdmc:/o10/",120,140,160);const char *items[]={".O10  demos",".SLR  compile+run",".LUA  (soon)",L("BACK","ZURUECK")};const int n=4;for(int i=0;i<n;i++){int y=72+i*22;if(i==sel){fill_rect(4,y-2,SCREEN_W-8,18,0,70,110);draw_text(left_x(items[i],12),y,items[i],255,255,120);}else draw_text(left_x(items[i],12),y,items[i],190,195,210);}draw_footer(L("A SELECT | B/X BACK","A WAEHLEN | B/X ZURUECK"));draw_bot_help(L("X = open this hub","X = dieses Menue"),L("HOME scripts","HOME Skripte"));drain();uint32_t k=wait_key();if(k&(BTN_B|BTN_X))return;if(k&BTN_UP){sel--;if(sel<0)sel=n-1;}if(k&BTN_DOWN){sel++;if(sel>=n)sel=0;}if(k&BTN_A){if(sel==0){int s2=0;while(1){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(".O10",12),40,".O10",200,220,255);const char *o[]={"HELLO.O10","SYSCHECK.O10",L("BACK","ZURUECK")};for(int i=0;i<3;i++){int y=80+i*24;if(i==s2){fill_rect(4,y-2,SCREEN_W-8,18,0,70,110);draw_text(left_x(o[i],12),y,o[i],255,255,120);}else draw_text(left_x(o[i],12),y,o[i],190,195,210);}draw_footer(L("A RUN | B BACK","A START | B ZURUECK"));drain();uint32_t k2=wait_key();if(k2&BTN_B)break;if(k2&BTN_UP){s2--;if(s2<0)s2=2;}if(k2&BTN_DOWN){s2++;if(s2>2)s2=0;}if(k2&BTN_A){if(s2==0)o10_run(demo_hello);else if(s2==1)o10_run(demo_info);else break;}}}else if(sel==1)screen_slr_run();else if(sel==2)screen_lua_stub();else return;}}}

static void screen_battery(void){while(1){battery_probe();clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("BATTERY","AKKU"),12),40,L("BATTERY","AKKU"),200,220,255);
{char buf[20];if(g_battery<0){draw_text(left_x(L("LEVEL: ?","STAND: ?"),12),80,L("LEVEL: ?","STAND: ?"),180,180,100);}
 else{int p=g_battery;int i=0;buf[i++]='L';buf[i++]='V';buf[i++]='L';buf[i++]=' ';if(p>=100){buf[i++]='1';buf[i++]='0';buf[i++]='0';}else if(p>=10){buf[i++]=(char)('0'+p/10);buf[i++]=(char)('0'+p%10);}else buf[i++]=(char)('0'+p);buf[i++]='%';buf[i]=0;draw_text(left_x(buf,12),80,buf,0,255,180);}}
draw_text(left_x(g_charging?L("CHARGING","LAEDT"):L("NOT CHARGING","LAEDT NICHT"),12),110,g_charging?L("CHARGING","LAEDT"):L("NOT CHARGING","LAEDT NICHT"),g_charging?80:180,g_charging?200:180,g_charging?255:100);
draw_text(left_x("MCU 0x0B / 0x0F",12),140,"MCU 0x0B / 0x0F",120,140,160);
draw_text(left_x(L("A REFRESH  B BACK","A AKTUALISIEREN  B ZURUECK"),12),180,L("A REFRESH  B BACK","A AKTUALISIEREN  B ZURUECK"),160,180,200);
draw_footer(L("A|B","A|B"));draw_bot_help(L("Battery via MCU","Akku ueber MCU"),NULL);drain();uint32_t k=wait_key();if(k&BTN_B)return;}}
static void screen_buttons(void){while(1){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("BUTTONS","TASTEN"),12),36,L("BUTTONS","TASTEN"),200,220,255);uint32_t k=pad_raw();
draw_text(left_x(k&BTN_A?"A DOWN":"A up",12),60,k&BTN_A?"A DOWN":"A up",0,255,120);
draw_text(left_x(k&BTN_B?"B DOWN":"B up",12),78,k&BTN_B?"B DOWN":"B up",0,255,120);
draw_text(left_x(k&BTN_X?"X DOWN":"X up",12),96,k&BTN_X?"X DOWN":"X up",0,255,120);
draw_text(left_x(k&BTN_Y?"Y DOWN":"Y up",12),114,k&BTN_Y?"Y DOWN":"Y up",0,255,120);
draw_text(left_x(k&BTN_L?"L DOWN":"L up",12),132,k&BTN_L?"L DOWN":"L up",0,255,120);
draw_text(left_x(k&BTN_R?"R DOWN":"R up",12),150,k&BTN_R?"R DOWN":"R up",0,255,120);
draw_text(left_x(k&BTN_START?"START DOWN":"START up",12),168,k&BTN_START?"START DOWN":"START up",0,255,120);
draw_text(left_x(L("HOLD B = EXIT","B HALTEN = ENDE"),12),200,L("HOLD B = EXIT","B HALTEN = ENDE"),160,180,200);
draw_footer(L("B EXIT","B ENDE"));drain();delay(8000);if(k&BTN_B){delay(25000);if(pad_raw()&BTN_B)return;}}}
static void screen_led(void){int mode=0;while(1){clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x("LED TEST",12),40,"LED TEST",200,220,255);const char *m[]={"WIFI LED ON","WIFI LED OFF","PWR BLUE","PWR RED"};draw_text(left_x(m[mode],12),90,m[mode],0,255,180);draw_text(left_x(L("UP/DOWN  A APPLY","HOCH/RUNTER  A AN"),12),130,L("UP/DOWN  A APPLY","HOCH/RUNTER  A AN"),160,180,200);draw_footer(L("A|B","A|B"));drain();uint32_t k=wait_key();if(k&BTN_B)return;if(k&BTN_UP){mode=(mode+3)%4;}if(k&BTN_DOWN){mode=(mode+1)%4;}if(k&BTN_A){if(mode==0)i2c_write_mcu(0x2A,0xF);else if(mode==1)i2c_write_mcu(0x2A,0);else if(mode==2)i2c_write_mcu(0x29,1);else i2c_write_mcu(0x29,4);}}}

static int g_fb_sel=0;
static const char *g_fb_names[]={
  "omni10/","luma/","3ds/","Nintendo 3DS/","Omni10.firm","version.dat","[SDMMC - live list soon]"
};
static const int g_fb_n=7;
static void screen_filebrowser(void){
  g_fb_sel=0;
  while(1){
    clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
    draw_text(left_x(L("FILE BROWSER","DATEIBROWSER"),12),34,L("FILE BROWSER","DATEIBROWSER"),200,220,255);
    draw_text(left_x("sdmc:/",12),50,"sdmc:/",120,140,160);
    int vis=5;int start=g_fb_sel-(vis/2);if(start<0)start=0;if(start>g_fb_n-vis)start=g_fb_n-vis;if(start<0)start=0;
    for(int i=0;i<vis&&start+i<g_fb_n;i++){
      int idx=start+i;int y=70+i*20;
      if(idx==g_fb_sel){fill_rect(4,y-2,SCREEN_W-8,18,0,70,110);draw_text(left_x(g_fb_names[idx],12),y,g_fb_names[idx],255,255,120);}
      else draw_text(left_x(g_fb_names[idx],12),y,g_fb_names[idx],190,195,210);
    }
    draw_text(left_x(L("A open  B back  Y root","A oeffnen  B zurueck"),12),190,L("A open  B back  Y root","A oeffnen  B zurueck"),140,160,180);
    draw_footer(L("FILES | B BACK","DATEIEN | B ZURUECK"));
    draw_bot_help(L("FIRM file browser","FIRM Dateibrowser"),L("SDMMC next","SDMMC als naechstes"));
    drain();
    uint32_t k=wait_key();
    if(k&BTN_B)return;
    if(k&BTN_UP){g_fb_sel--;if(g_fb_sel<0)g_fb_sel=g_fb_n-1;}
    if(k&BTN_DOWN){g_fb_sel++;if(g_fb_sel>=g_fb_n)g_fb_sel=0;}
    if(k&BTN_Y){g_fb_sel=0;}
    if(k&BTN_A){
      clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
      draw_text(left_x(g_fb_names[g_fb_sel],12),100,g_fb_names[g_fb_sel],0,255,180);
      draw_text(left_x(L("Need SDMMC+FAT for real IO","Braucht SDMMC+FAT"),12),130,L("Need SDMMC+FAT for real IO","Braucht SDMMC+FAT"),255,180,80);
      draw_footer(L("A/B BACK","A/B ZURUECK"));drain();
      while(1){uint32_t k2=wait_key();if(k2&(BTN_A|BTN_B))break;}
    }
  }
}
static int g_ftp_on=0;
static void screen_ftp(void){
  while(1){
    clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();
    draw_text(left_x("FTP SERVER",12),34,"FTP SERVER",200,220,255);
    draw_text(left_x(g_ftp_on?L("STATUS: RUN","STATUS: AN"):L("STATUS: STOP","STATUS: AUS"),12),60,g_ftp_on?L("STATUS: RUN","STATUS: AN"):L("STATUS: STOP","STATUS: AUS"),g_ftp_on?40:255,g_ftp_on?255:80,g_ftp_on?120:80);
    draw_text(left_x("PORT 5000",12),90,"PORT 5000",180,190,210);
    draw_text(left_x(L("Needs full WiFi stack in FIRM","Braucht WiFi-Stack in FIRM"),12),120,L("Needs full WiFi stack in FIRM","Braucht WiFi-Stack in FIRM"),255,180,80);
    draw_text(left_x(L("A toggle  B back","A umschalten  B zurueck"),12),160,L("A toggle  B back","A umschalten  B zurueck"),140,160,180);
    draw_footer(L("FTP | B BACK","FTP | B ZURUECK"));
    draw_bot_help(L("FIRM FTP scaffold","FIRM FTP Geruest"),g_wifi_status>0?L("NET ON","NET AN"):L("NET OFF","NET AUS"));
    drain();
    uint32_t k=wait_key();
    if(k&BTN_B)return;
    if(k&BTN_A){g_ftp_on=!g_ftp_on;}
  }
}
static void screen_menu(void){int sel=0;while(1){wifi_probe();battery_probe();g_ticks++;const char *items[]={L("ABOUT","INFO"),L("SYSTEM INFO","SYSTEMINFO"),L("HOME SCRIPTS","HOME SKRIPTE"),L("SETTINGS","EINSTELLUNGEN"),L("INTERNET / WIFI","INTERNET / WIFI"),L("BATTERY INFO","AKKU INFO"),L("BUTTON TEST","TASTEN TEST"),L("LED TEST","LED TEST"),L("FILE BROWSER","DATEIBROWSER"),L("FTP","FTP"),L("REBOOT","NEUSTART"),L("POWER OFF","AUSSCHALTEN")};const int n=12;clear_top(COL_BG_R,COL_BG_G,COL_BG_B);draw_header();draw_text(left_x(L("MAIN MENU","HAUPTMENUE"),12),36,L("MAIN MENU","HAUPTMENUE"),180,210,255);for(int i=0;i<n;i++){int y=52+i*17;if(i==sel){fill_rect(4,y-2,SCREEN_W-8,15,0,70,110);draw_text(left_x(items[i],16),y,items[i],255,255,120);}else draw_text(left_x(items[i],16),y,items[i],190,195,210);}draw_footer(L("A SELECT | X SCRIPTS | START+SELECT OFF","A | X SKRIPTE | START+SELECT AUS"));draw_bot_help(L("D-Pad move  X = scripts","Steuerkreuz  X = Skripte"),L("A select  B back","A waehlen  B zurueck"));drain();uint32_t k=wait_key();if(k&BTN_X){screen_scripts_hub();continue;}if(k&BTN_UP){sel--;if(sel<0)sel=n-1;}if(k&BTN_DOWN){sel++;if(sel>=n)sel=0;}if(k&BTN_A){switch(sel){case 0:screen_about();break;case 1:screen_sysinfo();break;case 2:screen_scripts_hub();break;case 3:screen_settings();break;case 4:screen_internet();break;case 5:screen_battery();break;case 6:screen_buttons();break;case 7:screen_led();break;case 8:screen_filebrowser();break;case 9:screen_ftp();break;case 10:if(confirm(L("REBOOT?","NEUSTART?"),L("Restart the console","Konsole neu starten")))reboot();break;case 11:if(confirm(L("POWER OFF?","AUSSCHALTEN?"),L("Turn console off","Konsole ausschalten")))power_off();break;}}}}
int main(int argc,char **argv){i2c_init();g_top[0]=FB_TOP0;g_top[1]=FB_TOP1;g_bot[0]=FB_BOT0;g_bot[1]=FB_BOT1;g_n_top=2;g_n_bot=2;if(argc>=2&&argv&&argv[1]){struct fb *fbs=(struct fb *)argv[1];g_n_top=0;g_n_bot=0;if(fbs[0].top_left)g_top[g_n_top++]=(volatile uint8_t*)fbs[0].top_left;if(fbs[1].top_left)g_top[g_n_top++]=(volatile uint8_t*)fbs[1].top_left;if(fbs[0].bottom)g_bot[g_n_bot++]=(volatile uint8_t*)fbs[0].bottom;if(fbs[1].bottom)g_bot[g_n_bot++]=(volatile uint8_t*)fbs[1].bottom;if(g_n_top==0){g_top[0]=FB_TOP0;g_top[1]=FB_TOP1;g_n_top=2;}}clear_top(8,8,24);clear_bot(8,8,24);draw_text_centered(90,"OMNI10",0,220,255);draw_text_centered(112,"BOOTING...",120,150,180);draw_text_centered(200,"V" OMNI_VERSION,80,100,120);draw_text_bot_centered(110,"FULL ACCESS",0,180,200);wifi_probe();battery_probe();drain();delay(700000);screen_menu();return 0;}
