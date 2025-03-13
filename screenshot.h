#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <codecvt>
#include "conversions.h"
#include "globals.h"


void GetScreenResolution(int& width, int& height);
Result takeScreenshot(const std::string& filename);


#endif