/*
 * O10-Inst-Booter — Omni10 Installer / Updater / Booter
 * Boot pattern inspired by TeamAuroraOS/AuroraOS-Installer.
 */
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#include "omni.h"
#include "ui.h"

static void copystr(char *dst, const char *src, size_t dstsz) {
	if (!dstsz) return;
	size_t n = strnlen(src, dstsz - 1);
	memcpy(dst, src, n);
	dst[n] = 0;
}

static void build_menu(App *app);
static void do_scan(App *app);
static void show_message(App *app, const char *title, const char *body, AppState ret);
static void trigger(App *app, Action a);

static void add_btn(App *app, const char *label, Action a, BtnStyle s) {
	if (app->nbtns >= MAX_BTNS) return;
	Btn *b = &app->btns[app->nbtns++];
	copystr(b->label, label, sizeof(b->label));
	b->action = a;
	b->style = s;
}

static void build_menu(App *app) {
	app->nbtns = 0;
	app->sel = 0;
	switch (app->state) {
	case ST_NEED_INSTALL:
		add_btn(app, "Install Omni10", ACT_DOWNLOAD, STYLE_PRIMARY);
		add_btn(app, "Other options...", ACT_OPTIONS, STYLE_NORMAL);
		break;
	case ST_INSTALLED:
	case ST_UPTODATE:
		add_btn(app, "Boot Omni10", ACT_BOOT, STYLE_PRIMARY);
		add_btn(app, "Update / Reinstall", ACT_UPDATE, STYLE_NORMAL);
		add_btn(app, "Uninstall Omni10", ACT_UNINSTALL, STYLE_DANGER);
		add_btn(app, "Other options...", ACT_OPTIONS, STYLE_NORMAL);
		break;
	case ST_OPTIONS:
		add_btn(app, "Refresh file check", ACT_REFRESH, STYLE_NORMAL);
		add_btn(app, "Exit", ACT_EXIT, STYLE_NORMAL);
		add_btn(app, "Back", ACT_BACK, STYLE_NORMAL);
		break;
	case ST_CONFIRM_UNINSTALL:
		add_btn(app, "Yes, uninstall", ACT_UNINSTALL_YES, STYLE_DANGER);
		add_btn(app, "Cancel", ACT_BACK, STYLE_NORMAL);
		break;
	case ST_MESSAGE:
		add_btn(app, "OK", ACT_MSG_OK, STYLE_PRIMARY);
		break;
	default:
		break;
	}
}

static void do_scan(App *app) {
	bool f = omni_firm_exists();
	app->installedVer[0] = 0;
	omni_read_local_version(app->installedVer, sizeof(app->installedVer));
	if (f) {
		app->state = ST_UPTODATE;
		if (app->installedVer[0])
			snprintf(app->statusLine, sizeof(app->statusLine),
			         "Omni10 ready (v%s).", app->installedVer);
		else
			copystr(app->statusLine, "Omni10.firm installed.", sizeof(app->statusLine));
	} else {
		app->state = ST_NEED_INSTALL;
		copystr(app->statusLine, "Omni10.firm not found on SD.", sizeof(app->statusLine));
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

static void on_progress(const char *label, int idx, int cnt, u32 done, u32 total, void *ud) {
	App *app = (App*)ud;
	copystr(app->dlLabel, label, sizeof(app->dlLabel));
	app->dlFileIndex = idx;
	app->dlFileCount = cnt;
	app->dlDone = done;
	app->dlTotal = total;
	uiRender(app);
}

static void run_install(App *app) {
	app->state = ST_DOWNLOADING;
	copystr(app->dlLabel, "Installing...", sizeof(app->dlLabel));
	uiRender(app);
	Result r = omni_install_from_urls(on_progress, app);
	do_scan(app);
	if (R_FAILED(r))
		show_message(app, "Install failed",
		             "Put Omni10.firm on SD root or sdmc:/omni10/, then retry. Luma3DS required.",
		             app->state);
	else
		show_message(app, "Omni10 installed",
		             "Omni10.firm is in luma/payloads. Choose Boot Omni10.",
		             app->state);
}

static void run_boot(App *app) {
	if (!omni_firm_exists()) {
		do_scan(app);
		show_message(app, "Omni10.firm missing", "Install Omni10 first.", app->state);
		return;
	}
	app->state = ST_BOOTING;
	app->bootRebooting = false;
	copystr(app->msgTitle, "Boot Omni10", sizeof(app->msgTitle));
	copystr(app->msgBody,
	        "Hold START and keep holding. Console reboots into Luma chainloader — keep START held, then choose Omni10. (B = cancel)",
	        sizeof(app->msgBody));

	while (aptMainLoop()) {
		hidScanInput();
		u32 down = hidKeysDown();
		u32 held = hidKeysHeld();
		if (down & KEY_B) { do_scan(app); return; }
		if (held & KEY_START) {
			app->bootRebooting = true;
			copystr(app->msgTitle, "Rebooting...", sizeof(app->msgTitle));
			copystr(app->msgBody,
			        "Keep holding START until Luma menu appears, then select Omni10.",
			        sizeof(app->msgBody));
			uiRender(app);
			Result r = omni_boot_reboot();
			if (R_FAILED(r)) {
				nsInit();
				NS_RebootSystem();
				nsExit();
			}
			while (aptMainLoop()) uiRender(app);
			return;
		}
		uiRender(app);
	}
}

static void trigger(App *app, Action a) {
	switch (a) {
	case ACT_DOWNLOAD:
	case ACT_UPDATE:
		run_install(app); break;
	case ACT_BOOT:
		run_boot(app); break;
	case ACT_UNINSTALL:
		app->returnState = ST_UPTODATE;
		app->state = ST_CONFIRM_UNINSTALL;
		copystr(app->msgTitle, "Uninstall Omni10?", sizeof(app->msgTitle));
		copystr(app->msgBody, "Deletes Omni10.firm from luma/payloads.", sizeof(app->msgBody));
		build_menu(app);
		break;
	case ACT_UNINSTALL_YES:
		omni_uninstall();
		do_scan(app);
		show_message(app, "Uninstalled", "Omni10.firm removed.", app->state);
		break;
	case ACT_OPTIONS:
		app->returnState = app->state;
		app->state = ST_OPTIONS;
		copystr(app->statusLine, "Other options", sizeof(app->statusLine));
		build_menu(app);
		break;
	case ACT_REFRESH:
		do_scan(app); break;
	case ACT_EXIT:
		app->running = false; break;
	case ACT_BACK:
	case ACT_MSG_OK:
		app->state = app->returnState;
		build_menu(app);
		break;
	default: break;
	}
}

static void handle_input(App *app, u32 kDown, const touchPosition *t) {
	if (app->state == ST_DOWNLOADING || app->state == ST_BOOTING) return;
	if (kDown & KEY_TOUCH) {
		int i = uiButtonAtTouch(app, t);
		if (i >= 0) { app->sel = i; trigger(app, app->btns[i].action); }
		return;
	}
	if (kDown & KEY_A) {
		if (app->nbtns > 0) trigger(app, app->btns[app->sel].action);
		return;
	}
	if (kDown & KEY_B) {
		if (app->state == ST_OPTIONS || app->state == ST_CONFIRM_UNINSTALL)
			trigger(app, ACT_BACK);
		else if (app->state == ST_MESSAGE)
			trigger(app, ACT_MSG_OK);
	}
	if (kDown & (KEY_DOWN | KEY_RIGHT))
		if (app->nbtns > 0) app->sel = (app->sel + 1) % app->nbtns;
	if (kDown & (KEY_UP | KEY_LEFT))
		if (app->nbtns > 0) app->sel = (app->sel + app->nbtns - 1) % app->nbtns;
	if (kDown & KEY_START)
		if (app->state == ST_NEED_INSTALL || app->state == ST_INSTALLED || app->state == ST_UPTODATE)
			app->running = false;
}

int main(int argc, char **argv) {
	(void)argc; (void)argv;
	osSetSpeedupEnable(true);
	romfsInit();
	uiInit();

	App app;
	memset(&app, 0, sizeof(app));
	app.running = true;
	do_scan(&app);

	while (app.running && aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		touchPosition touch;
		hidTouchRead(&touch);
		handle_input(&app, kDown, &touch);
		if (!app.running) break;
		uiRender(&app);
	}

	uiExit();
	romfsExit();
	return 0;
}
