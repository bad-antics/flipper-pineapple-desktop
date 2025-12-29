Flipper + Pineapple Desktop (C)

Overview
- Windows desktop app in C (Win32) to monitor Flipper Zero, list files, and probe WiFi Pineapple over USB.
- Features:
  - Flipper monitor: port, info, uptime, memory.
  - File explorer: list path, view file content.
  - Pineapple discovery: auto-detect 172.16.x.1 over USB; fetch /api/status.

Build (CMake + MSVC or MinGW)

PowerShell:
```
cd "C:\Users\pc123\OneDrive\Documents\pineflip\flipper-pineapple-desktop-c"
cmake -S . -B build
cmake --build build --config Release
```
Run:
```
build\Release\FlipperPineappleDesktopC.exe
```

Notes
- Serial COM enumeration scans COM1..COM40 using QueryDosDevice and opens first available port at 230400.
- Replace stub cJSON with official cJSON for real JSON parsing if needed.
- WinHTTP is used for Pineapple status probing; credentials/login can be added similarly.

Next
- Add upload/transfer to Flipper (storage write).
- Add context menu and sharing options in explorer dialog.
- Improve port filtering using VID/PID via SetupAPI.
