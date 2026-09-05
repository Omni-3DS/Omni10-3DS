/*
 * Minimal FTP-ish data listener for O10-Inst-Booter.
 * Full RFC FTP is large; this provides a TCP accept loop on port 5000
 * and a simple STATUS line for the UI. Extend toward real FTP commands.
 */
#include "ftp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Result ftp_init(FtpServer *s, int port) {
	memset(s, 0, sizeof(*s));
	s->port = port > 0 ? port : FTP_DEFAULT_PORT;
	s->listen_fd = -1;
	s->client_fd = -1;
	snprintf(s->status, sizeof(s->status), "FTP idle");
	return 0;
}

void ftp_exit(FtpServer *s) {
	ftp_stop(s);
}

Result ftp_start(FtpServer *s) {
	ftp_stop(s);
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		snprintf(s->status, sizeof(s->status), "socket() failed");
		return -1;
	}
	int yes = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons((u16)s->port);
	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		close(fd);
		snprintf(s->status, sizeof(s->status), "bind :%d failed", s->port);
		return -2;
	}
	if (listen(fd, 1) < 0) {
		close(fd);
		snprintf(s->status, sizeof(s->status), "listen failed");
		return -3;
	}
	/* non-blocking accept */
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	s->listen_fd = fd;
	s->running = true;
	snprintf(s->status, sizeof(s->status), "FTP listen *:%d (WiFi on)", s->port);
	return 0;
}

void ftp_stop(FtpServer *s) {
	if (s->client_fd >= 0) { close(s->client_fd); s->client_fd = -1; }
	if (s->listen_fd >= 0) { close(s->listen_fd); s->listen_fd = -1; }
	s->running = false;
	snprintf(s->status, sizeof(s->status), "FTP stopped");
}

void ftp_poll(FtpServer *s) {
	if (!s->running || s->listen_fd < 0) return;
	struct sockaddr_in cli;
	socklen_t cl = sizeof(cli);
	int c = accept(s->listen_fd, (struct sockaddr*)&cli, &cl);
	if (c >= 0) {
		if (s->client_fd >= 0) close(s->client_fd);
		s->client_fd = c;
		const char *hello =
			"220 Omni10 O10-Inst-Booter FTP-ready (minimal)\r\n";
		send(c, hello, strlen(hello), 0);
		snprintf(s->status, sizeof(s->status), "Client %s connected",
		         inet_ntoa(cli.sin_addr));
	}
	if (s->client_fd >= 0) {
		char buf[256];
		int n = recv(s->client_fd, buf, sizeof(buf) - 1, MSG_DONTWAIT);
		if (n > 0) {
			buf[n] = 0;
			s->bytes_rx += (u32)n;
			/* Minimal command handling */
			if (strncmp(buf, "QUIT", 4) == 0) {
				const char *bye = "221 Bye\r\n";
				send(s->client_fd, bye, strlen(bye), 0);
				close(s->client_fd);
				s->client_fd = -1;
				snprintf(s->status, sizeof(s->status), "Client quit");
			} else if (strncmp(buf, "USER", 4) == 0 || strncmp(buf, "PASS", 4) == 0) {
				const char *ok = "230 OK\r\n";
				send(s->client_fd, ok, strlen(ok), 0);
				s->bytes_tx += 6;
			} else if (strncmp(buf, "SYST", 4) == 0) {
				const char *sys = "215 UNIX Type: L8\r\n";
				send(s->client_fd, sys, strlen(sys), 0);
			} else if (strncmp(buf, "PWD", 3) == 0) {
				const char *pwd = "257 \"/\" is current directory.\r\n";
				send(s->client_fd, pwd, strlen(pwd), 0);
			} else if (strncmp(buf, "TYPE", 4) == 0) {
				const char *ok = "200 OK\r\n";
				send(s->client_fd, ok, strlen(ok), 0);
			} else {
				const char *un = "502 Not implemented yet\r\n";
				send(s->client_fd, un, strlen(un), 0);
			}
		} else if (n == 0) {
			close(s->client_fd);
			s->client_fd = -1;
			snprintf(s->status, sizeof(s->status), "Client disconnected");
		}
	}
}
