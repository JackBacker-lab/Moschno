#include "screenshot.h"

/*
// Function to get screen resolution
void GetScreenResolution(int& width, int& height) {
	DEVMODE devmode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

	width = devmode.dmPelsWidth;
	height = devmode.dmPelsHeight;
}


// Function for creating a screenshot
void takeScreenshot(const char* filename) {
	int screenX, screenY;
	GetScreenResolution(screenX, screenY);

	HDC hScreenDC = GetDC(NULL);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
	SelectObject(hMemoryDC, hBitmap);

	if (!BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY)) {
		std::cerr << "BitBlt failed!" << std::endl;
	}

	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = screenX;
	bi.biHeight = -screenY;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((screenX * bi.biBitCount + 31) / 32) * 4 * screenY;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char* lpbitmap = (char*)GlobalLock(hDIB);

	if (GetDIBits(hMemoryDC, hBitmap, 0, (UINT)screenY, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
		std::cerr << "GetDIBits failed!" << std::endl;
	}

	std::wstring wide_filename = std::wstring(filename, filename + strlen(filename));
	LPCWSTR w_filename = wide_filename.c_str();

	HANDLE hFile = CreateFile(w_filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;

	DWORD dwBytesWritten;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, nullptr);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, nullptr);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, nullptr);

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	CloseHandle(hFile);

	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
}
*/


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
void takeScreenshot(const std::wstring& filename) {
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
    HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
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
