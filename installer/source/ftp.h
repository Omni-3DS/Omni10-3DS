#pragma once
#include <3ds.h>
#include <stdbool.h>

#define FTP_DEFAULT_PORT 5000

typedef struct {
	bool running;
	int port;
	int listen_fd;
	int client_fd;
	char status[96];
	u32 bytes_tx;
	u32 bytes_rx;
} FtpServer;

Result ftp_init(FtpServer *s, int port);
void ftp_exit(FtpServer *s);
Result ftp_start(FtpServer *s);
void ftp_stop(FtpServer *s);
/* Call each frame while running */
void ftp_poll(FtpServer *s);
