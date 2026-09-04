/*
 * Omni10 – Lua host stub (planned embed).
 * Full VM not linked yet; API reserved for future port.
 */

#include "lua_omni.h"

omni_result_t lua_omni_init(void)
{
    return OMNI_ERR_GENERIC; /* not embedded yet */
}

void lua_omni_deinit(void)
{
}

omni_result_t lua_omni_run_file(const char *path)
{
    (void)path;
    return OMNI_ERR_GENERIC;
}

omni_result_t lua_omni_run_string(const char *code)
{
    (void)code;
    return OMNI_ERR_GENERIC;
}

omni_result_t lua_omni_register_host(void)
{
    return OMNI_ERR_GENERIC;
}
