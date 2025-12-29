#pragma once
#include <windows.h>
#include <stdbool.h>

bool flipper_auto_connect(char* outPort, size_t outLen);
bool flipper_send_command(const char* cmd, char* out, size_t outLen);
void flipper_open_explorer_dialog(HWND parent);
