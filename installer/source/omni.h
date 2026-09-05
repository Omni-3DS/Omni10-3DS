#pragma once
#include <3ds.h>
#include <stdbool.h>
#include <stddef.h>

#define OMNI_FIRM_PATH "sdmc:/luma/payloads/Omni10.firm"
#define OMNI_VER_PATH  "sdmc:/omni10/version.dat"
#define OMNI_DIR       "sdmc:/omni10"
#define LUMA_PAYLOADS  "sdmc:/luma/payloads"

bool omni_firm_exists(void);
bool omni_read_local_version(char *out, size_t outsz);
Result omni_install_from_urls(void (*progress)(const char*, int, int, u32, u32, void*), void *ud);
Result omni_uninstall(void);
Result omni_boot_reboot(void);
u32 omni_wifi_status(void);
