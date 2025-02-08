#include "conversions.h"

// Function for conversion UTF-16 → UTF-8 (std::string → std::wstring)
std::string wstringToUtf8(const std::wstring& wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}


// Function for conversion UTF-8 → UTF-16 (std::wstring → std::string)
std::wstring utf8_to_wstring(const std::string& utf8_str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
	if (size_needed == 0) {
		return L"";
	}
	std::wstring wstr(size_needed - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}