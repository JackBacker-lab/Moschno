#ifndef STARTUP_H
#define STARTUP_H

#include <iostream>
#include <shlobj.h>

void AddToStartup(const std::wstring& appName, const std::wstring& exePath);

#endif
