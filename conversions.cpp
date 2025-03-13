#include "conversions.h"

// Function for conversion UTF-16 → UTF-8 (std::wstring → std::string)
std::string wstringToUtf8(const std::wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    if (size_needed == 0) {
        return "";
    }
    std::vector<char> buffer(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), buffer.data(), size_needed, NULL, NULL);
    return std::string(buffer.begin(), buffer.end());
}

// Function for conversion UTF-8 → UTF-16 (std::string → std::wstring)
std::wstring utf8_to_wstring(const std::string& utf8_str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    if (size_needed == 0) {
        return L"";
    }
    std::vector<wchar_t> buffer(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, buffer.data(), size_needed);
    return std::wstring(buffer.begin(), buffer.end() - 1);  // исключаем завершающий нулевой символ
}
