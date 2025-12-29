#include "pineapple.h"
#include <windows.h>
#include <winhttp.h>
#include <iphlpapi.h>
#include <stdio.h>
#pragma comment(lib, "iphlpapi.lib")

static char g_base[64] = "http://172.16.42.1:1471";

static bool probe(const wchar_t* host, INTERNET_PORT port, const wchar_t* path) {
    HINTERNET h = WinHttpOpen(L"FlipperDesktop/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!h) return false;
    HINTERNET c = WinHttpConnect(h, host, port, 0);
    if (!c) { WinHttpCloseHandle(h); return false; }
    HINTERNET r = WinHttpOpenRequest(c, L"GET", path, NULL, WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!r) { WinHttpCloseHandle(c); WinHttpCloseHandle(h); return false; }
    BOOL ok = WinHttpSendRequest(r, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                 WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(r, NULL);
    DWORD status = 0; DWORD len = sizeof(status);
    if (ok) WinHttpQueryHeaders(r, WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &len, WINHTTP_NO_HEADER_INDEX);
    WinHttpCloseHandle(r); WinHttpCloseHandle(c); WinHttpCloseHandle(h);
    return ok && (status == 200 || status == 401 || status == 403);
}

bool pineapple_discover(void) {
    // Use IP Helper API to find adapters with 172.16.x.x
    ULONG sz = 0; GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST|GAA_FLAG_SKIP_DNS_SERVER, NULL, NULL, &sz);
    IP_ADAPTER_ADDRESSES* addrs = (IP_ADAPTER_ADDRESSES*)malloc(sz);
    if (!addrs) return false;
    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST|GAA_FLAG_SKIP_DNS_SERVER, NULL, addrs, &sz) != NO_ERROR) {
        free(addrs); return false;
    }
    for (IP_ADAPTER_ADDRESSES* a = addrs; a; a = a->Next) {
        IP_ADAPTER_UNICAST_ADDRESS* u = a->FirstUnicastAddress;
        for (; u; u = u->Next) {
            SOCKADDR_IN* sin = (SOCKADDR_IN*)u->Address.lpSockaddr;
            unsigned char* b = (unsigned char*)&sin->sin_addr.S_un.S_addr;
            char ip[32]; snprintf(ip, sizeof(ip), "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
            if (b[0] == 172 && b[1] == 16) {
                wchar_t host[32]; swprintf(host, 32, L"172.16.%u.1", b[2]);
                if (probe(host, 1471, L"/api/status")) {
                    snprintf(g_base, sizeof(g_base), "http://172.16.%u.1:1471", b[2]);
                    free(addrs); return true;
                }
                if (probe(host, 80, L"/api/status")) {
                    snprintf(g_base, sizeof(g_base), "http://172.16.%u.1", b[2]);
                    free(addrs); return true;
                }
            }
        }
    }
    free(addrs);
    return false;
}

bool pineapple_status_json(char* out, int outlen) {
    wchar_t host[64]; INTERNET_PORT port = 80; const wchar_t* path = L"/api/status";
    // parse g_base into host and port
    if (strncmp(g_base, "http://", 7) == 0) {
        const char* p = g_base + 7; const char* colon = strchr(p, ':'); const char* slash = strchr(p, '/');
        char h[64] = {0};
        if (colon && (!slash || colon < slash)) {
            memcpy(h, p, colon - p);
            port = (INTERNET_PORT)atoi(colon+1);
        } else if (slash) {
            memcpy(h, p, slash - p);
        } else {
            snprintf(h, sizeof(h), "%s", p);
        }
        swprintf(host, 64, L"%S", h);
        if (!slash) path = L"/api/status";
    } else {
        swprintf(host, 64, L"%S", g_base);
    }

    HINTERNET h = WinHttpOpen(L"FlipperDesktop/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!h) return false;
    HINTERNET c = WinHttpConnect(h, host, port, 0);
    if (!c) { WinHttpCloseHandle(h); return false; }
    HINTERNET r = WinHttpOpenRequest(c, L"GET", path, NULL, WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!r) { WinHttpCloseHandle(c); WinHttpCloseHandle(h); return false; }
    BOOL ok = WinHttpSendRequest(r, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                 WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(r, NULL);
    DWORD status = 0; DWORD len = sizeof(status);
    if (ok) WinHttpQueryHeaders(r, WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &status, &len, WINHTTP_NO_HEADER_INDEX);
    if (!ok || status != 200) { WinHttpCloseHandle(r); WinHttpCloseHandle(c); WinHttpCloseHandle(h); return false; }
    DWORD avail = 0, read = 0, total = 0;
    do {
        WinHttpQueryDataAvailable(r, &avail);
        if (!avail) break;
        if (total + avail >= (DWORD)outlen) avail = (DWORD)outlen - total - 1;
        if (!WinHttpReadData(r, out + total, avail, &read)) break;
        total += read;
    } while (read > 0);
    out[total] = '\0';
    WinHttpCloseHandle(r); WinHttpCloseHandle(c); WinHttpCloseHandle(h);
    return true;
}
