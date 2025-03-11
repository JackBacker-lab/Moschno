#include "screenshot.h"


// Получение разрешения экрана
void GetScreenResolution(int& width, int& height) {
    DEVMODE devmode{};
    if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devmode)) {
        width = devmode.dmPelsWidth;
        height = devmode.dmPelsHeight;
    }
    else {
        std::cerr << "Failed to get screen resolution!" << std::endl;
        width = height = 0;
    }
}


// Создание скриншота и сохранение в BMP-файл
void takeScreenshot(const std::string& filename) {
    int screenX, screenY;
    GetScreenResolution(screenX, screenY);
    if (screenX == 0 || screenY == 0) return;

    // Создание контекстов
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        std::cerr << "CreateCompatibleDC failed!" << std::endl;
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    // Создание битмапа
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    if (!hBitmap) {
        std::cerr << "CreateCompatibleBitmap failed!" << std::endl;
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    SelectObject(hMemoryDC, hBitmap);

    // Захват экрана
    if (!BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY)) {
        std::cerr << "BitBlt failed!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    // Настройка заголовков BMP
    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = screenX;
    bi.biHeight = -screenY;  // Отразить изображение
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;

    DWORD dwBmpSize = screenX * screenY * 4;

    std::vector<char> buffer(dwBmpSize);
    if (GetDIBits(hMemoryDC, hBitmap, 0, screenY, buffer.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
        std::cerr << "GetDIBits failed!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    // Создание файла
    HANDLE hFile = CreateFileW(utf8_to_wstring(filename).c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    // Заголовок файла BMP
    BITMAPFILEHEADER bmfHeader{};
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    DWORD bytesWritten;
    WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, nullptr);
    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &bytesWritten, nullptr);
    WriteFile(hFile, buffer.data(), dwBmpSize, &bytesWritten, nullptr);

    // Закрытие ресурсов
    CloseHandle(hFile);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);
}
