#pragma once
#include <3ds.h>

/* Notification LED helpers (MCU) */
void led_init(void);
void led_exit(void);
void led_set_rgb(u8 r, u8 g, u8 b);
void led_off(void);
void led_color_index(int idx); /* 0=red 1=green 2=blue 3=yellow 4=white */
