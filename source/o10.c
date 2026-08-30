/*
 * Omni10-3DS – .o10 Script Engine (stub)
 */

#include "o10.h"
#include <string.h>

omni_result_t o10_init(void)
{
    return OMNI_OK;
}

void o10_deinit(void)
{
}

omni_result_t o10_run_file(const char* path)
{
    (void)path;
    // TODO: Load file and parse/execute .o10 script
    return OMNI_ERR_GENERIC;
}

omni_result_t o10_run_string(const char* script)
{
    (void)script;
    // TODO: Parse and execute
    return OMNI_ERR_GENERIC;
}

bool o10_is_script(const char* path)
{
    if (!path) return false;
    size_t len = strlen(path);
    if (len < 4) return false;
    return (strcmp(path + len - 4, ".o10") == 0);
}
