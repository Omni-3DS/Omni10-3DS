#pragma once

#include "common.h"

/*
 * Omni10 Networking / FTP interface
 * This is one of the main features that sets Omni10 apart.
 */

typedef struct {
    bool running;
    int port;
    char user[32];
    char pass[32];
    int clients;
} ftp_status_t;

omni_result_t net_init(void);
void net_deinit(void);

bool net_is_connected(void);

omni_result_t net_ftp_start(int port, const char* user, const char* pass);
omni_result_t net_ftp_stop(void);
omni_result_t net_ftp_status(ftp_status_t* status);
