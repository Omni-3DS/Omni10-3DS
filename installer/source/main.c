/*
 * O10-Inst-Booter — Installer / Updater / Booter
 * R4/DSTT: backup to o10/r4|dstt/backup.bin BEFORE warning, full restore.
 */
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#include "omni.h"
#include "ui.h"
#include "led.h"
#include "backup.h"

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

static void fill_ip(App *app) {
	copystr(app->localIp, "ip", sizeof(app->localIp));
}

static void shuffle_colors(App *app) {
	u8 base[COLOR_SEQ_LEN] = {0, 1, 2, 3, 4};
	int i, j;
	u8 t;
	for (i = COLOR_SEQ_LEN - 1; i > 0; i--) {
		j = rand() % (i + 1);
		t = base[i]; base[i] = base[j]; base[j] = t;
	}
	memcpy(app->colorSeq, base, COLOR_SEQ_LEN);
	app->colorIndex = 0;
	app->colorTick = osGetTime();
}

static const char *color_name(int idx) {
	switch (idx) {
	case 0: return "RED";
	case 1: return "GREEN";
	case 2: return "BLUE";
	case 3: return "YELLOW";
	case 4: return "WHITE";
	default: return "?";
	}
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
		add_btn(app, "Flashcart R4 / DSTT", ACT_FLASHCART, STYLE_DANGER);
		add_btn(app, "Refresh file check", ACT_REFRESH, STYLE_NORMAL);
		add_btn(app, "Exit", ACT_EXIT, STYLE_NORMAL);
		add_btn(app, "Back", ACT_BACK, STYLE_NORMAL);
		break;
	case ST_FLASHCART_MENU:
		add_btn(app, "R4: backup + warning", ACT_R4_WARN, STYLE_DANGER);
		add_btn(app, "DSTT: backup + warning", ACT_DSTT_WARN, STYLE_DANGER);
		add_btn(app, "Restore R4 backup", ACT_RESTORE_R4, STYLE_PRIMARY);
		add_btn(app, "Restore DSTT backup", ACT_RESTORE_DSTT, STYLE_PRIMARY);
		add_btn(app, "Back", ACT_BACK, STYLE_NORMAL);
		break;
	case ST_FLASHCART_WARN:
		add_btn(app, "A = Continue", ACT_WARN_CONTINUE, STYLE_PRIMARY);
		add_btn(app, "Cancel", ACT_BACK, STYLE_NORMAL);
		break;
	case ST_COLOR_SEQ:
		add_btn(app, "Done / Cancel", ACT_COLOR_DONE, STYLE_NORMAL);
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

/* Backup FIRST, then warning screen */
static void open_flashcart_warn(App *app, BackupKind kind) {
	char bmsg[160];
	Result br;
	fill_ip(app);
	app->backupKind = kind;
	app->returnState = ST_FLASHCART_MENU;
	br = backup_create(kind, bmsg, sizeof(bmsg));
	if (R_FAILED(br)) {
		show_message(app, "Backup failed", bmsg, ST_FLASHCART_MENU);
		return;
	}
	app->state = ST_FLASHCART_WARN;
	snprintf(app->msgTitle, sizeof(app->msgTitle),
	         kind == BACKUP_KIND_DSTT ? "WARNING — DSTT" : "WARNING — R4");
	snprintf(app->msgBody, sizeof(app->msgBody),
	         "%s\nPlease get a magnet and activate sleep mode then enter in browser "
	         "http://%s:1089 — Press A to continue.",
	         bmsg, app->localIp[0] ? app->localIp : "ip");
	snprintf(app->statusLine, sizeof(app->statusLine),
	         "Backup saved: %s", backup_path(kind));
	build_menu(app);
}

static void run_restore(App *app, BackupKind kind) {
	char bmsg[160];
	Result r = backup_restore(kind, bmsg, sizeof(bmsg));
	do_scan(app);
	if (R_FAILED(r))
		show_message(app, "Restore failed", bmsg, ST_FLASHCART_MENU);
	else
		show_message(app, "Restore OK", bmsg, ST_FLASHCART_MENU);
}

static void start_color_seq(App *app) {
	shuffle_colors(app);
	app->state = ST_COLOR_SEQ;
	app->returnState = ST_FLASHCART_MENU;
	copystr(app->msgTitle, "Color sequence", sizeof(app->msgTitle));
	snprintf(app->msgBody, sizeof(app->msgBody),
	         "Every 2s a color on NOTIFICATION LED (5 colors). "
	         "Order them on server http://%s:1089 to confirm.",
	         app->localIp[0] ? app->localIp : "ip");
	led_color_index(app->colorSeq[0]);
	build_menu(app);
}

static void tick_color_seq(App *app) {
	u64 now;
	if (app->state != ST_COLOR_SEQ) return;
	now = osGetTime();
	if (now - app->colorTick < 2000) return;
	app->colorTick = now;
	app->colorIndex = (app->colorIndex + 1) % COLOR_SEQ_LEN;
	led_color_index(app->colorSeq[app->colorIndex]);
	snprintf(app->statusLine, sizeof(app->statusLine),
	         "LED %d/5: %s  (match order on server)",
	         app->colorIndex + 1, color_name(app->colorSeq[app->colorIndex]));
}

static void on_progress(const char *label, int idx, int cnt, u32 done, u32 total, void *ud) {
	App *app = (App *)ud;
	copystr(app->dlLabel, label, sizeof(app->dlLabel));
	app->dlFileIndex = idx;
	app->dlFileCount = cnt;
	app->dlDone = done;
	app->dlTotal = total;
	uiRender(app);
}

static void run_install(App *app) {
	Result r;
	app->state = ST_DOWNLOADING;
	copystr(app->dlLabel, "Installing...", sizeof(app->dlLabel));
	uiRender(app);
	r = omni_install_from_urls(on_progress, app);
	do_scan(app);
	if (R_FAILED(r))
		show_message(app, "Install failed",
		             "Put Omni10.firm on SD root or sdmc:/omni10/, then retry.",
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
	copystr(app->msgTitle, "Boot Omni10", sizeof(app->msgTitle));
	copystr(app->msgBody,
	        "Hold START and keep holding. Reboots into Luma chainloader — choose Omni10. (B = cancel)",
	        sizeof(app->msgBody));
	while (aptMainLoop()) {
		hidScanInput();
		if (hidKeysDown() & KEY_B) { do_scan(app); return; }
		if (hidKeysHeld() & KEY_START) {
			app->bootRebooting = true;
			copystr(app->msgTitle, "Rebooting...", sizeof(app->msgTitle));
			uiRender(app);
			omni_boot_reboot();
			nsInit(); NS_RebootSystem(); nsExit();
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
	case ACT_FLASHCART:
		app->returnState = ST_OPTIONS;
		app->state = ST_FLASHCART_MENU;
		copystr(app->statusLine, "R4 / DSTT backup + warn + restore", sizeof(app->statusLine));
		build_menu(app);
		break;
	case ACT_R4_WARN:
		open_flashcart_warn(app, BACKUP_KIND_R4);
		break;
	case ACT_DSTT_WARN:
		open_flashcart_warn(app, BACKUP_KIND_DSTT);
		break;
	case ACT_RESTORE_R4:
		run_restore(app, BACKUP_KIND_R4);
		break;
	case ACT_RESTORE_DSTT:
		run_restore(app, BACKUP_KIND_DSTT);
		break;
	case ACT_WARN_CONTINUE:
		start_color_seq(app);
		break;
	case ACT_COLOR_DONE:
		led_off();
		app->state = app->returnState;
		build_menu(app);
		break;
	case ACT_REFRESH:
		do_scan(app); break;
	case ACT_EXIT:
		app->running = false; break;
	case ACT_BACK:
	case ACT_MSG_OK:
		led_off();
		app->state = app->returnState;
		build_menu(app);
		break;
	default: break;
	}
}

static void handle_input(App *app, u32 kDown, const touchPosition *t) {
	if (app->state == ST_DOWNLOADING || app->state == ST_BOOTING) return;
	if (app->state == ST_COLOR_SEQ) {
		if (kDown & (KEY_B | KEY_A | KEY_START))
			trigger(app, ACT_COLOR_DONE);
		return;
	}
	if (kDown & KEY_TOUCH) {
		int i = uiButtonAtTouch(app, t);
		if (i >= 0) { app->sel = i; trigger(app, app->btns[i].action); }
		return;
	}
	if (kDown & KEY_A) {
		if (app->state == ST_FLASHCART_WARN) { trigger(app, ACT_WARN_CONTINUE); return; }
		if (app->nbtns > 0) trigger(app, app->btns[app->sel].action);
		return;
	}
	if (kDown & KEY_B) {
		if (app->state == ST_OPTIONS || app->state == ST_CONFIRM_UNINSTALL ||
		    app->state == ST_FLASHCART_WARN || app->state == ST_FLASHCART_MENU)
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
	App app;
	(void)argc; (void)argv;
	osSetSpeedupEnable(true);
	srand((unsigned)osGetTime());
	romfsInit();
	led_init();
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
			tick_color_seq(&app);
			handle_input(&app, kDown, &touch);
		}
		if (!app.running) break;
		uiRender(&app);
	}
	led_exit();
	uiExit();
	romfsExit();
	return 0;
}
