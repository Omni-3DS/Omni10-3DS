/*
 * Omni10-3DS – Networking / FTP implementation (stub)
 *
 * Real FTP will run on the ARM11 side.
 */

#include "net.h"
#include <string.h>

static ftp_status_t g_ftp = {0};

omni_result_t net_init(void)
{
    memset(&g_ftp, 0, sizeof(g_ftp));
    return OMNI_OK;
}

void net_deinit(void)
{
    if (g_ftp.running) {
        net_ftp_stop();
    }
}

bool net_is_connected(void)
{
    // TODO: Check real network status
    return false;
}

omni_result_t net_ftp_start(int port, const char* user, const char* pass)
{
    if (g_ftp.running)
        return OMNI_ERR_GENERIC;

    g_ftp.port = port > 0 ? port : 5000;
    strncpy(g_ftp.user, user ? user : "omni", sizeof(g_ftp.user) - 1);
    strncpy(g_ftp.pass, pass ? pass : "omni10", sizeof(g_ftp.pass) - 1);
    g_ftp.running = true;
    g_ftp.clients = 0;

    // TODO: Actually start FTP server on ARM11

    return OMNI_OK;
}

omni_result_t net_ftp_stop(void)
{
    if (!g_ftp.running)
        return OMNI_OK;

    // TODO: Stop server
    g_ftp.running = false;
    g_ftp.clients = 0;

    return OMNI_OK;
}

omni_result_t net_ftp_status(ftp_status_t* status)
{
    if (!status)
        return OMNI_ERR_INVALID;

    *status = g_ftp;
    return OMNI_OK;
}
