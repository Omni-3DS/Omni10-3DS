#pragma once
#include <3ds.h>
#include <stdbool.h>

#define MAX_BTNS 6

typedef enum {
	ST_HOME,
	ST_DOWNLOADING,
	ST_BOOTING,
	ST_MESSAGE
} AppState;

typedef enum {
	ACT_INSTALL,
	ACT_BOOT,
	ACT_EXIT,
	ACT_MSG_OK
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
	char msgBody[256];
	char dlLabel[64];
	u32 dlDone, dlTotal;
	bool bootRebooting;
	char installedVer[32];
	bool firmOk;
} App;
