#pragma once
#include <windows.h>
#include <stdbool.h>

bool serial_find_first_port(char* outPort, size_t outLen);
HANDLE serial_open(const char* port, DWORD baud);
bool serial_send_line(HANDLE h, const char* line);
int  serial_read_available(HANDLE h, char* buf, int maxlen, int timeout_ms);
void serial_close(HANDLE h);
