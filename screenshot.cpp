#include "screenshot.h"


static void GetScreenResolution(int& width, int& height) {
    DEVMODE devmode{};
    if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devmode)) {
        width = devmode.dmPelsWidth;
        height = devmode.dmPelsHeight;
    }
}

static CLSID GetEncoderClsid(const WCHAR* format) {
    UINT num = 0;
    UINT size = 0;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return CLSID{};

    pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo*>(malloc(size));
    if (!pImageCodecInfo) return CLSID{};

    memset(pImageCodecInfo, 0, size);
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    CLSID clsid{};
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            clsid = pImageCodecInfo[i].Clsid;
            break;
        }
    }
    free(pImageCodecInfo);
    return clsid;
}

Result takeScreenshot(const std::wstring& filename) {
    int scaleFactor = 2;
    int screenX, screenY;
    GetScreenResolution(screenX, screenY);
    if (screenX == 0 || screenY == 0) return { COE::TakeScreenshotError, "Error getting screenshot resolution.", ResponseType::None, ""};

    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    if (!hBitmap) {
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "Error making Bitmap.", ResponseType::None, ""};
    }

    SelectObject(hMemoryDC, hBitmap);
    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok) {
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(nullptr, hScreenDC);
        return { COE::TakeScreenshotError, "", ResponseType::None, ""};
    }

    {
        Gdiplus::Bitmap bitmap(hBitmap, nullptr);

        int newWidth = screenX / scaleFactor;
        int newHeight = screenY / scaleFactor;

        if (newWidth <= 0 || newHeight <= 0) {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            DeleteObject(hBitmap);
            DeleteDC(hMemoryDC);
            ReleaseDC(nullptr, hScreenDC);
            return { COE::TakeScreenshotError, "Resolution <= 0.", ResponseType::None, ""};
        }

        Gdiplus::Bitmap resizedBitmap(newWidth, newHeight, PixelFormat32bppARGB);
        Gdiplus::Graphics graphics(&resizedBitmap);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.DrawImage(&bitmap, 0, 0, newWidth, newHeight);

        CLSID pngClsid = GetEncoderClsid(L"image/png");
        if (resizedBitmap.Save(filename.c_str(), &pngClsid, nullptr) != Gdiplus::Ok) {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            DeleteObject(hBitmap);
            DeleteDC(hMemoryDC);
            ReleaseDC(nullptr, hScreenDC);
            return { COE::TakeScreenshotError, "", ResponseType::None, ""};
        }
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);

    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);
    return { COE::Success, "", ResponseType::None, ""};
}
