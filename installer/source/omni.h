#pragma once
#include <3ds.h>
#include <stdbool.h>
#include <stddef.h>

#define OMNI_FIRM_PATH "sdmc:/luma/payloads/Omni10.firm"
#define OMNI_VER_PATH  "sdmc:/o10/version.dat"
#define OMNI_DIR       "sdmc:/o10"
#define LUMA_PAYLOADS  "sdmc:/luma/payloads"

#define OMNI_FIRM_URL \
	"https://github.com/Omni-3DS/Omni10-3DS/releases/latest/download/Omni10.firm"
#define OMNI_VER_URL \
	"https://github.com/Omni-3DS/Omni10-3DS/releases/latest/download/version.dat"

bool omni_firm_exists(void);
bool omni_read_local_version(char *out, size_t outsz);
void omni_ensure_dirs(void);

/* Download latest firm from GitHub and install to luma/payloads */
Result omni_install_latest(void (*progress)(const char *label, u32 done, u32 total, void *ud),
                           void *ud);

Result omni_boot_reboot(void);
