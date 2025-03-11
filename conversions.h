#ifndef CONVERSIONS_H
#define CONVERSIONS_H	

#include <iostream>
#include <windows.h>
#include <vector>

std::string wstringToUtf8(const std::wstring& wstr);

std::wstring utf8_to_wstring(const std::string& utf8_str);

#endif