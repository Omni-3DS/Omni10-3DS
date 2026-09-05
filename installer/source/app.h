#pragma once
#include <3ds.h>
#include <stdbool.h>
#include "fsutil.h"
#include "ftp.h"

#define MAX_BTNS 10
#define COLOR_SEQ_LEN 5

typedef enum {
	ST_NEED_INSTALL,
	ST_INSTALLED,
	ST_UPTODATE,
	ST_OPTIONS,
	ST_DOWNLOADING,
	ST_BOOTING,
	ST_MESSAGE,
	ST_CONFIRM_UNINSTALL,
	ST_FLASHCART_WARN,
	ST_COLOR_SEQ,
	ST_FILE_BROWSER,
	ST_FTP
} AppState;

typedef enum {
	ACT_DOWNLOAD,
	ACT_UPDATE,
	ACT_BOOT,
	ACT_UNINSTALL,
	ACT_UNINSTALL_YES,
	ACT_OPTIONS,
	ACT_REFRESH,
	ACT_EXIT,
	ACT_BACK,
	ACT_MSG_OK,
	ACT_FLASHCART,
	ACT_WARN_CONTINUE,
	ACT_COLOR_DONE,
	ACT_FILE_BROWSER,
	ACT_FTP,
	ACT_FTP_TOGGLE
} Action;

typedef enum {
	STYLE_NORMAL,
	STYLE_PRIMARY,
	STYLE_DANGER
} BtnStyle;

typedef struct {
	char label[48];
	Action action;
	BtnStyle style;
} Btn;

typedef struct {
	bool running;
	AppState state;
	AppState returnState;
	int sel;
	int nbtns;
	Btn btns[MAX_BTNS];
	char statusLine[128];
	char msgTitle[64];
	char msgBody[320];
	char dlLabel[64];
	int dlFileIndex, dlFileCount;
	u32 dlDone, dlTotal;
	bool bootRebooting;
	char installedVer[32];
	char remoteVer[32];
	int colorIndex;
	u8 colorSeq[COLOR_SEQ_LEN];
	u64 colorTick;
	char localIp[24];
	FsBrowser browser;
	FtpServer ftp;
	bool soc_ok;
} App;
