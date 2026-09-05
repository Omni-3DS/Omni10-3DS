#pragma once
#include "app.h"

void uiInit(void);
void uiExit(void);
void uiRender(App *app);
int uiButtonAtTouch(App *app, const touchPosition *t);
