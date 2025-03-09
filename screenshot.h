#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>

void GetScreenResolution(int& width, int& height);
void takeScreenshot(const std::wstring& filename);

#endif