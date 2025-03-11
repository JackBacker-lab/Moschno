#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <codecvt>
#include "conversions.h"


void GetScreenResolution(int& width, int& height);
void takeScreenshot(const std::string& filename);


#endif