#include "led.h"
#include <string.h>

static int s_ok;

void led_init(void) {
	s_ok = R_SUCCEEDED(mcuHwcInit()) ? 1 : 0;
}

void led_exit(void) {
	if (s_ok) {
		led_off();
		mcuHwcExit();
		s_ok = 0;
	}
}

void led_set_rgb(u8 r, u8 g, u8 b) {
	InfoLedPattern pat;
	int i;
	if (!s_ok) return;
	memset(&pat, 0, sizeof(pat));
	pat.delay = 0x10;      /* ~1s step */
	pat.smoothing = 0x20;
	pat.loopDelay = 0x00;
	pat.blinkSpeed = 0x00;
	for (i = 0; i < 32; i++) {
		pat.redPattern[i] = r;
		pat.greenPattern[i] = g;
		pat.bluePattern[i] = b;
	}
	MCUHWC_SetInfoLedPattern(&pat);
}

void led_off(void) {
	led_set_rgb(0, 0, 0);
}

void led_color_index(int idx) {
	switch (idx) {
	case 0: led_set_rgb(255, 0, 0); break;
	case 1: led_set_rgb(0, 255, 0); break;
	case 2: led_set_rgb(0, 80, 255); break;
	case 3: led_set_rgb(255, 220, 0); break;
	case 4: led_set_rgb(255, 255, 255); break;
	default: led_off(); break;
	}
}
