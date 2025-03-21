#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <codecvt>
#include "conversions.h"
#include "globals.h"
#include <gdiplus.h>
#include <memory>

#pragma comment (lib, "gdiplus.lib")


Result takeScreenshot(const std::wstring& filename);


#endif