#include "screenshot.h"


void GetScreenResolution(int& width, int& height) {
    DEVMODE devmode{};
    if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devmode)) {
        width = devmode.dmPelsWidth;
        height = devmode.dmPelsHeight;
    }
    else {
        width = height = 0;
    }
}


// Taking screenshot and saving it into a BMP-file
Result takeScreenshot(const std::string& filename) {
    int screenX, screenY;
    GetScreenResolution(screenX, screenY);
    if (screenX == 0 || screenY == 0) 
        return { COE::TakeScreenshotError, "Failed to get screen resolution!", ResponseType::None, "" };

    // Creating contexts
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "CreateCompatibleDC failed!", ResponseType::None, "" };
    }

    // Creating Bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    if (!hBitmap) {
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "CreateCompatibleBitmap failed!", ResponseType::None, "" };
    }

    SelectObject(hMemoryDC, hBitmap);

    // Screen capture
    if (!BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY)) {
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "BitBlt failed!", ResponseType::None, "" };
    }

    // Setting up BMP headers
    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = screenX;
    bi.biHeight = -screenY;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;

    DWORD dwBmpSize = screenX * screenY * 4;

    std::vector<char> buffer(dwBmpSize);
    if (GetDIBits(hMemoryDC, hBitmap, 0, screenY, buffer.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "GetDIBits failed!", ResponseType::None, "" };
    }

    // Creating file
    HANDLE hFile = CreateFileW(utf8_to_wstring(filename).c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "CreateFile failed!", ResponseType::None, "" };
    }

    // BMP-file headers
    BITMAPFILEHEADER bmfHeader{};
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    DWORD bytesWritten;
    WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, nullptr);
    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &bytesWritten, nullptr);
    WriteFile(hFile, buffer.data(), dwBmpSize, &bytesWritten, nullptr);

    // Closing resources
    CloseHandle(hFile);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);
    return { COE::Success, "", ResponseType::Text, "Screenshot has been taken successfully." };
}
