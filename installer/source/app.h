#pragma once
#include <3ds.h>
#include <stdbool.h>

#define MAX_BTNS 8
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
	ST_COLOR_SEQ
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
	ACT_COLOR_DONE
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
	/* flashcart color sequence */
	int colorIndex;
	u8 colorSeq[COLOR_SEQ_LEN]; /* 0=R 1=G 2=B 3=Y 4=W */
	u64 colorTick;
	char localIp[24];
} App;
