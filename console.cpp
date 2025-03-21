#include "console.h"

void HideConsole() {
    HWND hwnd = GetConsoleWindow();
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
    }
    FreeConsole();
}

void ShowConsole() {
    FreeConsole();
    AllocConsole();

    // Redirect standard streams to a new console
    HWND hwnd = GetConsoleWindow();
    //EnableWindow(hwnd, FALSE);
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
}
