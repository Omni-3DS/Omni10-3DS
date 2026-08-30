/*
 * Omni10-3DS – System module (stub)
 */

#include "sys.h"
#include <string.h>

omni_result_t sys_init(void)
{
    return OMNI_OK;
}

void sys_deinit(void)
{
}

omni_result_t sys_get_info(sys_info_t* info)
{
    if (!info) return OMNI_ERR_INVALID;

    // Placeholder values – real detection comes later
    info->model = "Unknown";
    info->region = "Unknown";
    info->serial = "Unknown";
    info->is_new3ds = false;
    info->is_devkit = false;

    return OMNI_OK;
}

const char* sys_version(void)
{
    return OMNI10_VERSION_STRING;
}

void sys_reboot(void)
{
    // TODO: Trigger real reboot
}

void sys_poweroff(void)
{
    // TODO: Trigger real poweroff
}

omni_result_t sys_set_brightness(int level)
{
    if (level < 0 || level > 5) return OMNI_ERR_INVALID;
    // TODO: Set brightness
    return OMNI_OK;
}
