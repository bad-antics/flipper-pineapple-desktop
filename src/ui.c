#include "ui.h"
#include "flipper.h"
#include "pineapple.h"
#include <stdio.h>

static HWND hEditMonitor;

void ui_init(HWND hwnd) {
    CreateWindowEx(0, L"BUTTON", L"Refresh Monitor", WS_CHILD|WS_VISIBLE,
                   10, 10, 150, 30, hwnd, (HMENU)ID_BTN_REFRESH_MON, NULL, NULL);
    CreateWindowEx(0, L"BUTTON", L"Open File Explorer", WS_CHILD|WS_VISIBLE,
                   170, 10, 170, 30, hwnd, (HMENU)ID_BTN_SHOW_EXPLORER, NULL, NULL);

    hEditMonitor = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
                   WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL,
                   10, 50, 960, 580, hwnd, (HMENU)ID_EDIT_MONITOR, NULL, NULL);
}

static void set_monitor_text(const wchar_t* text) {
    SetWindowTextW(hEditMonitor, text);
}

static void refresh_monitor(HWND hwnd) {
    char port[32] = {0};
    if (!flipper_auto_connect(port, sizeof(port))) {
        set_monitor_text(L"Flipper Zero not connected.");
        return;
    }
    char info[4096] = {0};
    char uptime[1024] = {0};
    char memory[1024] = {0};
    flipper_send_command("info device", info, sizeof(info));
    flipper_send_command("uptime", uptime, sizeof(uptime));
    flipper_send_command("free", memory, sizeof(memory));

    wchar_t buf[8192];
    swprintf(buf, 8192, L"Port: %S\r\n\r\nInfo:\r\n%S\r\n\r\nUptime: %S\r\nMemory: %S\r\n",
             port, info, uptime, memory);
    set_monitor_text(buf);
}

static void open_explorer(HWND hwnd) {
    pineapple_discover(); // opportunistic network discovery
    flipper_open_explorer_dialog(hwnd);
}

LRESULT ui_handle_command(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(wParam)) {
    case ID_BTN_REFRESH_MON:
        refresh_monitor(hwnd);
        return 0;
    case ID_BTN_SHOW_EXPLORER:
        open_explorer(hwnd);
        return 0;
    }
    return 1;
}
