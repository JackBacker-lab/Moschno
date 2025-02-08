#include "screenshot.h"

// Функция для получения разрешения экрана
void GetScreenResolution(int& width, int& height) {
	// Получаем описание текущего монитора
	DEVMODE devmode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

	// Сохраняем разрешение
	width = devmode.dmPelsWidth;
	height = devmode.dmPelsHeight;
}

// Функция для создания скриншота экрана
void takeScreenshot(const char* filename) {
	int screenX, screenY;
	GetScreenResolution(screenX, screenY);  // Получаем разрешение экрана

	std::cout << "Screen Resolution: " << screenX << "x" << screenY << std::endl; // Выводим разрешение экрана

	// Создаем DC для экрана и совместимый DC
	HDC hScreenDC = GetDC(NULL);  // Получаем контекст устройства экрана
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);  // Создаем совместимый контекст для рисования в памяти

	// Создаем bitmap для хранения изображения
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);  // Создаем битмап
	SelectObject(hMemoryDC, hBitmap);  // Выбираем его в контекст памяти

	// Копируем содержимое экрана в память
	if (!BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY)) {
		std::cerr << "BitBlt failed!" << std::endl;
	}

	// Сохраняем bitmap в файл
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = screenX;
	bi.biHeight = -screenY; // Отрицательное значение для переворота
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((screenX * bi.biBitCount + 31) / 32) * 4 * screenY;

	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);  // Выделяем память для изображения
	char* lpbitmap = (char*)GlobalLock(hDIB);

	if (GetDIBits(hMemoryDC, hBitmap, 0, (UINT)screenY, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
		std::cerr << "GetDIBits failed!" << std::endl;
	}

	// Преобразуем filename в LPCWSTR для функции Windows
	std::wstring wide_filename = std::wstring(filename, filename + strlen(filename));
	LPCWSTR w_filename = wide_filename.c_str();

	HANDLE hFile = CreateFile(w_filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);  // Открываем файл для записи

	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;  // "BM" в десятичном виде

	DWORD dwBytesWritten;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, nullptr);  // Записываем заголовок файла
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, nullptr);  // Записываем заголовок DIB
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, nullptr);  // Записываем данные изображения

	// Освобождаем память
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	CloseHandle(hFile);

	// Освобождаем ресурсы
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);

	std::cout << "Screenshot saved to " << filename << std::endl;
}