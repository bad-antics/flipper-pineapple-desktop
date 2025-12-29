#include "flipper.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>

static HANDLE g_hSerial = INVALID_HANDLE_VALUE;
static char g_port[32] = {0};

bool flipper_auto_connect(char* outPort, size_t outLen) {
    if (g_hSerial != INVALID_HANDLE_VALUE) {
        snprintf(outPort, outLen, "%s", g_port);
        return true;
    }
    char port[32];
    if (!serial_find_first_port(port, sizeof(port))) return false;
    HANDLE h = serial_open(port, 230400);
    if (h == INVALID_HANDLE_VALUE) return false;
    g_hSerial = h;
    snprintf(g_port, sizeof(g_port), "%s", port);
    snprintf(outPort, outLen, "%s", port);
    Sleep(100);
    return true;
}

bool flipper_send_command(const char* cmd, char* out, size_t outLen) {
    if (g_hSerial == INVALID_HANDLE_VALUE) return false;
    if (!serial_send_line(g_hSerial, cmd)) return false;
    Sleep(600);
    int n = serial_read_available(g_hSerial, out, (int)outLen-1, 500);
    return n >= 0;
}

static INT_PTR CALLBACK ExplorerDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG:
        SetDlgItemTextA(hDlg, 1001, "/ext");
        return TRUE;
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case 1002: { // List
            char path[256]; GetDlgItemTextA(hDlg, 1001, path, sizeof(path));
            char cmd[256]; snprintf(cmd, sizeof(cmd), "storage list %s", path);
            char out[8192] = {0};
            if (!flipper_send_command(cmd, out, sizeof(out))) snprintf(out, sizeof(out), "Error or unsupported");
            SetDlgItemTextA(hDlg, 1003, out);
            return TRUE;
        }
        case 1004: { // View
            char path[256]; GetDlgItemTextA(hDlg, 1005, path, sizeof(path));
            char cmd[256]; snprintf(cmd, sizeof(cmd), "storage read %s", path);
            char out[8192] = {0};
            if (!flipper_send_command(cmd, out, sizeof(out))) snprintf(out, sizeof(out), "Error or unsupported");
            SetDlgItemTextA(hDlg, 1003, out);
            return TRUE;
        }
        case 1006: EndDialog(hDlg, 0); return TRUE; // Close
        }
        break;
    }
    }
    return FALSE;
}

void flipper_open_explorer_dialog(HWND parent) {
    // Simple dialog via CreateDialogIndirect
    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);
    HWND hDlg = CreateWindowEx(WS_EX_DLGMODALFRAME, L"STATIC", L"Flipper File Explorer",
        WS_CAPTION|WS_SYSMENU|WS_VISIBLE,
        100, 100, 700, 500, parent, NULL, hInst, NULL);

    CreateWindowEx(0, L"STATIC", L"Path:", WS_CHILD|WS_VISIBLE, 10, 10, 40, 24, hDlg, NULL, hInst, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"/ext", WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
                   60, 10, 400, 24, hDlg, (HMENU)1001, hInst, NULL);
    CreateWindowEx(0, L"BUTTON", L"List", WS_CHILD|WS_VISIBLE,
                   470, 10, 60, 24, hDlg, (HMENU)1002, hInst, NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
                   WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_VSCROLL|ES_AUTOVSCROLL,
                   10, 40, 660, 380, hDlg, (HMENU)1003, hInst, NULL);

    CreateWindowEx(0, L"STATIC", L"File:", WS_CHILD|WS_VISIBLE, 10, 430, 40, 24, hDlg, NULL, hInst, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
                   60, 430, 400, 24, hDlg, (HMENU)1005, hInst, NULL);

    CreateWindowEx(0, L"BUTTON", L"View", WS_CHILD|WS_VISIBLE,
                   470, 430, 60, 24, hDlg, (HMENU)1004, hInst, NULL);
    CreateWindowEx(0, L"BUTTON", L"Close", WS_CHILD|WS_VISIBLE,
                   610, 430, 60, 24, hDlg, (HMENU)1006, hInst, NULL);
}
