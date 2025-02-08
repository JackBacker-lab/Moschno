#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <iostream>
#include <windows.h>

void GetScreenResolution(int& width, int& height);
void takeScreenshot(const char* filename);

#endif