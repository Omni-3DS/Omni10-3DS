#pragma once

#include "common.h"

/*
 * Omni10 UI interface (placeholder)
 */

void ui_init(void);
void ui_deinit(void);

void ui_echo(const char* msg);
bool ui_ask(const char* msg);
int  ui_ask_selection(const char* title, const char** options, int count);
char* ui_ask_input(const char* prompt, const char* default_value);

void ui_progress(const char* title, int current, int total);
void ui_notify(const char* msg);

void ui_clear(void);
void ui_draw_status(const char* text);
