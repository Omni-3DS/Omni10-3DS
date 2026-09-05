#include "ui.h"
#include <citro2d.h>
#include <stdio.h>
#include <string.h>

static C3D_RenderTarget *top, *bottom;
static C2D_TextBuf g_buf;

void uiInit(void) {
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	g_buf = C2D_TextBufNew(4096);
}

void uiExit(void) {
	C2D_TextBufDelete(g_buf);
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

static void draw_text(float x, float y, float scale, u32 color, const char *s) {
	C2D_Text t;
	C2D_TextBufClear(g_buf);
	C2D_TextParse(&t, g_buf, s);
	C2D_TextOptimize(&t);
	C2D_DrawText(&t, C2D_WithColor, x, y, 0.5f, scale, scale, color);
}

void uiRender(App *app) {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, C2D_Color32(12, 16, 36, 255));
	C2D_SceneBegin(top);
	draw_text(12, 12, 0.7f, C2D_Color32(0, 220, 255, 255), "O10-Inst-Booter");
	draw_text(12, 40, 0.5f, C2D_Color32(200, 210, 230, 255), "Omni10 Installer / Updater / Booter");
	draw_text(12, 70, 0.5f, C2D_Color32(180, 190, 210, 255), app->statusLine);
	if (app->installedVer[0]) {
		char line[64];
		snprintf(line, sizeof(line), "Installed: v%s", app->installedVer);
		draw_text(12, 95, 0.45f, C2D_Color32(100, 255, 160, 255), line);
	}
	if (app->state == ST_MESSAGE || app->state == ST_BOOTING || app->state == ST_CONFIRM_UNINSTALL) {
		draw_text(12, 130, 0.55f, C2D_Color32(255, 220, 100, 255), app->msgTitle);
		draw_text(12, 160, 0.4f, C2D_Color32(220, 220, 230, 255), app->msgBody);
	}
	if (app->state == ST_DOWNLOADING)
		draw_text(12, 150, 0.5f, C2D_Color32(255, 200, 80, 255), app->dlLabel);

	C2D_TargetClear(bottom, C2D_Color32(18, 22, 40, 255));
	C2D_SceneBegin(bottom);
	for (int i = 0; i < app->nbtns; i++) {
		float y = 20.0f + i * 42.0f;
		u32 bg = (i == app->sel) ? C2D_Color32(0, 90, 140, 255) : C2D_Color32(30, 40, 60, 255);
		if (app->btns[i].style == STYLE_DANGER)
			bg = (i == app->sel) ? C2D_Color32(160, 40, 40, 255) : C2D_Color32(80, 30, 30, 255);
		if (app->btns[i].style == STYLE_PRIMARY)
			bg = (i == app->sel) ? C2D_Color32(0, 140, 90, 255) : C2D_Color32(20, 70, 50, 255);
		C2D_DrawRectSolid(16, y, 0.4f, 288, 36, bg);
		draw_text(28, y + 8, 0.5f, C2D_Color32(255, 255, 255, 255), app->btns[i].label);
	}
	draw_text(16, 220, 0.35f, C2D_Color32(140, 150, 170, 255), "A select  B back  START exit");
	C3D_FrameEnd(0);
}

int uiButtonAtTouch(App *app, const touchPosition *t) {
	if (!t || t->px == 0) return -1;
	for (int i = 0; i < app->nbtns; i++) {
		float y = 20.0f + i * 42.0f;
		if (t->px >= 16 && t->px <= 304 && t->py >= y && t->py <= y + 36)
			return i;
	}
	return -1;
}
