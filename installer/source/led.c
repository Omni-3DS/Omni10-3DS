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
	if (!s_ok) return;
	/* Info LED animation pattern: simple solid via MCUHWC */
	u8 pattern[32];
	memset(pattern, 0, sizeof(pattern));
	/* libctru InfoLedPattern layout varies; write RGB intensity bytes */
	pattern[0] = r;
	pattern[1] = g;
	pattern[2] = b;
	pattern[3] = 0xFF; /* delay / enable-ish */
	MCUHWC_SetInfoLedPattern(pattern);
}

void led_off(void) {
	led_set_rgb(0, 0, 0);
}

void led_color_index(int idx) {
	switch (idx) {
	case 0: led_set_rgb(255, 0, 0); break;     /* red */
	case 1: led_set_rgb(0, 255, 0); break;     /* green */
	case 2: led_set_rgb(0, 80, 255); break;    /* blue */
	case 3: led_set_rgb(255, 220, 0); break;   /* yellow */
	case 4: led_set_rgb(255, 255, 255); break; /* white */
	default: led_off(); break;
	}
}
