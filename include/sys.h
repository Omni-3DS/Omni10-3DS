#pragma once

#include "common.h"

/*
 * System information & control
 */

typedef struct {
    const char* model;      // "Old 3DS", "New 3DS", etc.
    const char* region;     // "EUR", "USA", "JPN"...
    const char* serial;
    bool is_new3ds;
    bool is_devkit;
} sys_info_t;

omni_result_t sys_init(void);
void sys_deinit(void);

omni_result_t sys_get_info(sys_info_t* info);
const char* sys_version(void);

void sys_reboot(void);
void sys_poweroff(void);

omni_result_t sys_set_brightness(int level); // 0-5
