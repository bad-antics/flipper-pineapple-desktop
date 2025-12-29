#pragma once
#include <windows.h>

// Control IDs
#define ID_BTN_REFRESH_MON   101
#define ID_BTN_SHOW_EXPLORER 102
#define ID_EDIT_MONITOR      103

void ui_init(HWND hwnd);
LRESULT ui_handle_command(HWND hwnd, WPARAM wParam, LPARAM lParam);
