/*
 * Omni10-3DS – UI implementation (stub)
 */

#include "ui.h"
#include <stdio.h>

void ui_init(void)
{
    // TODO: Initialize framebuffer, font, etc.
}

void ui_deinit(void)
{
}

void ui_echo(const char* msg)
{
    // Placeholder – real implementation will draw on screen
    (void)msg;
}

bool ui_ask(const char* msg)
{
    (void)msg;
    return true; // placeholder
}

int ui_ask_selection(const char* title, const char** options, int count)
{
    (void)title;
    (void)options;
    (void)count;
    return 0;
}

char* ui_ask_input(const char* prompt, const char* default_value)
{
    (void)prompt;
    (void)default_value;
    return NULL;
}

void ui_progress(const char* title, int current, int total)
{
    (void)title;
    (void)current;
    (void)total;
}

void ui_notify(const char* msg)
{
    (void)msg;
}

void ui_clear(void)
{
}

void ui_draw_status(const char* text)
{
    (void)text;
}
