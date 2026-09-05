/*
 * O10-Inst-Booter — only Install/Update (latest firm) + Boot
 */
#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include "app.h"
#include "omni.h"
#include "http.h"
#include "ui.h"

static void copystr(char *dst, const char *src, size_t dstsz) {
	size_t n;
	if (!dstsz) return;
	n = strnlen(src, dstsz - 1);
	memcpy(dst, src, n);
	dst[n] = 0;
}

static void build_menu(App *app);
static void do_scan(App *app);
static void show_message(App *app, const char *title, const char *body, AppState ret);
static void trigger(App *app, Action a);

static void add_btn(App *app, const char *label, Action a, BtnStyle s) {
	Btn *b;
	if (app->nbtns >= MAX_BTNS) return;
	b = &app->btns[app->nbtns++];
	copystr(b->label, label, sizeof(b->label));
	b->action = a;
	b->style = s;
}

static void build_menu(App *app) {
	app->nbtns = 0;
	app->sel = 0;
	if (app->state == ST_MESSAGE) {
		add_btn(app, "OK", ACT_MSG_OK, STYLE_PRIMARY);
		return;
	}
	if (app->state != ST_HOME) return;
	/* Only three actions */
	add_btn(app, app->firmOk ? "Update Omni10" : "Install Omni10", ACT_INSTALL, STYLE_PRIMARY);
	if (app->firmOk)
		add_btn(app, "Boot Omni10", ACT_BOOT, STYLE_PRIMARY);
	add_btn(app, "Exit", ACT_EXIT, STYLE_NORMAL);
}

static void do_scan(App *app) {
	app->firmOk = omni_firm_exists();
	app->installedVer[0] = 0;
	omni_read_local_version(app->installedVer, sizeof(app->installedVer));
	app->state = ST_HOME;
	if (app->firmOk) {
		if (app->installedVer[0])
			snprintf(app->statusLine, sizeof(app->statusLine),
			         "Ready — v%s. Update or Boot.", app->installedVer);
		else
			copystr(app->statusLine, "Omni10.firm found. Update or Boot.", sizeof(app->statusLine));
	} else {
		copystr(app->statusLine, "Not installed. Install downloads latest firm.",
		        sizeof(app->statusLine));
	}
	build_menu(app);
}

static void show_message(App *app, const char *title, const char *body, AppState ret) {
	app->returnState = ret;
	copystr(app->msgTitle, title, sizeof(app->msgTitle));
	copystr(app->msgBody, body, sizeof(app->msgBody));
	app->state = ST_MESSAGE;
	build_menu(app);
}

static void on_progress(const char *label, u32 done, u32 total, void *ud) {
	App *app = (App *)ud;
	copystr(app->dlLabel, label, sizeof(app->dlLabel));
	app->dlDone = done;
	app->dlTotal = total;
	uiRender(app);
}

static void run_install(App *app) {
	Result r;
	app->state = ST_DOWNLOADING;
	copystr(app->dlLabel, "Connecting...", sizeof(app->dlLabel));
	app->dlDone = 0;
	app->dlTotal = 0;
	uiRender(app);
	r = omni_install_latest(on_progress, app);
	do_scan(app);
	if (R_FAILED(r))
		show_message(app, "Install failed",
		             "Need Wi-Fi, or put Omni10.firm on SD root and retry.",
		             ST_HOME);
	else
		show_message(app, "OK",
		             "Latest Omni10.firm installed. You can Boot now.",
		             ST_HOME);
}

static void run_boot(App *app) {
	if (!omni_firm_exists()) {
		do_scan(app);
		show_message(app, "Missing firm", "Install first.", ST_HOME);
		return;
	}
	app->state = ST_BOOTING;
	copystr(app->msgTitle, "Boot Omni10", sizeof(app->msgTitle));
	copystr(app->msgBody,
	        "Hold START. Console reboots into Luma — keep START held, select Omni10. (B = cancel)",
	        sizeof(app->msgBody));
	while (aptMainLoop()) {
		hidScanInput();
		if (hidKeysDown() & KEY_B) {
			do_scan(app);
			return;
		}
		if (hidKeysHeld() & KEY_START) {
			app->bootRebooting = true;
			copystr(app->msgTitle, "Rebooting...", sizeof(app->msgTitle));
			uiRender(app);
			omni_boot_reboot();
			nsInit();
			NS_RebootSystem();
			nsExit();
			while (aptMainLoop())
				uiRender(app);
			return;
		}
		uiRender(app);
	}
}

static void trigger(App *app, Action a) {
	switch (a) {
	case ACT_INSTALL:
		run_install(app);
		break;
	case ACT_BOOT:
		run_boot(app);
		break;
	case ACT_EXIT:
		app->running = false;
		break;
	case ACT_MSG_OK:
		app->state = ST_HOME;
		do_scan(app);
		break;
	default:
		break;
	}
}

static void handle_input(App *app, u32 kDown, const touchPosition *t) {
	if (app->state == ST_DOWNLOADING || app->state == ST_BOOTING) return;
	if (kDown & KEY_TOUCH) {
		int i = uiButtonAtTouch(app, t);
		if (i >= 0) {
			app->sel = i;
			trigger(app, app->btns[i].action);
		}
		return;
	}
	if (kDown & KEY_A) {
		if (app->nbtns > 0) trigger(app, app->btns[app->sel].action);
		return;
	}
	if (kDown & (KEY_DOWN | KEY_RIGHT))
		if (app->nbtns > 0) app->sel = (app->sel + 1) % app->nbtns;
	if (kDown & (KEY_UP | KEY_LEFT))
		if (app->nbtns > 0) app->sel = (app->sel + app->nbtns - 1) % app->nbtns;
	if (kDown & KEY_START)
		app->running = false;
}

int main(int argc, char **argv) {
	App app;
	(void)argc;
	(void)argv;
	osSetSpeedupEnable(true);
	romfsInit();
	httpInit();
	uiInit();
	memset(&app, 0, sizeof(app));
	app.running = true;
	do_scan(&app);
	while (app.running && aptMainLoop()) {
		hidScanInput();
		{
			u32 kDown = hidKeysDown();
			touchPosition touch;
			hidTouchRead(&touch);
			handle_input(&app, kDown, &touch);
		}
		if (!app.running) break;
		uiRender(&app);
	}
	uiExit();
	httpExit();
	romfsExit();
	return 0;
}
