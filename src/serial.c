#include "serial.h"
#include <stdio.h>

bool serial_find_first_port(char* outPort, size_t outLen) {
    for (int i = 1; i <= 40; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "COM%d", i);
        char device[32];
        snprintf(device, sizeof(device), "\\\\.\\%s", name);
        char target[256];
        if (QueryDosDeviceA(name, target, sizeof(target))) {
            snprintf(outPort, outLen, "%s", device);
            return true;
        }
    }
    return false;
}

HANDLE serial_open(const char* port, DWORD baud) {
    HANDLE h = CreateFileA(port, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(h, &dcb)) { CloseHandle(h); return INVALID_HANDLE_VALUE; }
    dcb.BaudRate = baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    if (!SetCommState(h, &dcb)) { CloseHandle(h); return INVALID_HANDLE_VALUE; }
    COMMTIMEOUTS to = {0};
    to.ReadIntervalTimeout = 50;
    to.ReadTotalTimeoutMultiplier = 10;
    to.ReadTotalTimeoutConstant = 50;
    to.WriteTotalTimeoutMultiplier = 10;
    to.WriteTotalTimeoutConstant = 50;
    SetCommTimeouts(h, &to);
    return h;
}

bool serial_send_line(HANDLE h, const char* line) {
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    char buf[1024];
    int n = snprintf(buf, sizeof(buf), "%s\r\n", line);
    return WriteFile(h, buf, (DWORD)n, &written, NULL) && (int)written == n;
}

int serial_read_available(HANDLE h, char* buf, int maxlen, int timeout_ms) {
    if (h == INVALID_HANDLE_VALUE) return -1;
    DWORD total = 0;
    DWORD start = GetTickCount();
    while ((GetTickCount() - start) < (DWORD)timeout_ms) {
        DWORD errs = 0; 
        COMSTAT st = {0};
        ClearCommError(h, &errs, &st);
        if (st.cbInQue > 0) {
            DWORD toRead = st.cbInQue > (DWORD)maxlen ? (DWORD)maxlen : st.cbInQue;
            DWORD read = 0;
            if (!ReadFile(h, buf+total, toRead, &read, NULL)) break;
            total += read;
            if (total >= (DWORD)maxlen) break;
        } else {
            Sleep(20);
        }
    }
    if (total < (DWORD)maxlen) buf[total] = '\0';
    return (int)total;
}

void serial_close(HANDLE h) {
    if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
}
