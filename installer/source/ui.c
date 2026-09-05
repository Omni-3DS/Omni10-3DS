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
	draw_text(12, 16, 0.7f, C2D_Color32(0, 220, 255, 255), "Omni10");
	draw_text(12, 48, 0.5f, C2D_Color32(200, 210, 230, 255), "Install · Update · Boot");
	draw_text(12, 80, 0.45f, C2D_Color32(180, 190, 210, 255), app->statusLine);
	if (app->installedVer[0]) {
		char line[64];
		snprintf(line, sizeof(line), "Installed: v%s", app->installedVer);
		draw_text(12, 105, 0.45f, C2D_Color32(100, 255, 160, 255), line);
	}
	if (app->state == ST_MESSAGE || app->state == ST_BOOTING) {
		draw_text(12, 140, 0.55f, C2D_Color32(255, 220, 100, 255), app->msgTitle);
		draw_text(12, 170, 0.4f, C2D_Color32(220, 220, 230, 255), app->msgBody);
	}
	if (app->state == ST_DOWNLOADING) {
		draw_text(12, 150, 0.5f, C2D_Color32(255, 200, 80, 255), app->dlLabel);
		if (app->dlTotal > 0) {
			char p[48];
			u32 pct = (app->dlDone * 100u) / app->dlTotal;
			snprintf(p, sizeof(p), "%u%%", (unsigned)pct);
			draw_text(12, 180, 0.5f, C2D_Color32(180, 220, 255, 255), p);
		}
	}

	C2D_TargetClear(bottom, C2D_Color32(18, 22, 40, 255));
	C2D_SceneBegin(bottom);
	{
		int i;
		for (i = 0; i < app->nbtns; i++) {
			float y = 28.0f + i * 50.0f;
			u32 bg = (i == app->sel) ? C2D_Color32(0, 100, 150, 255) : C2D_Color32(30, 40, 60, 255);
			if (app->btns[i].style == STYLE_PRIMARY)
				bg = (i == app->sel) ? C2D_Color32(0, 150, 100, 255) : C2D_Color32(20, 80, 55, 255);
			C2D_DrawRectSolid(20, y, 0.4f, 280, 42, bg);
			draw_text(32, y + 10, 0.55f, C2D_Color32(255, 255, 255, 255), app->btns[i].label);
		}
	}
	draw_text(20, 220, 0.35f, C2D_Color32(140, 150, 170, 255), "A select   START exit");
	C3D_FrameEnd(0);
}

int uiButtonAtTouch(App *app, const touchPosition *t) {
	int i;
	if (!t || t->px == 0) return -1;
	for (i = 0; i < app->nbtns; i++) {
		float y = 28.0f + i * 50.0f;
		if (t->px >= 20 && t->px <= 300 && t->py >= y && t->py <= y + 42)
			return i;
	}
	return -1;
}
