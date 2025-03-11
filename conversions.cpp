#include "conversions.h"

// Function for conversion UTF-16 → UTF-8 (std::wstring → std::string)
std::string wstringToUtf8(const std::wstring& wstr) {
    // Определяем необходимый размер буфера для строки UTF-8
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    if (size_needed == 0) {
        return ""; // Если преобразование не удалось, возвращаем пустую строку
    }

    // Используем vector для автоматического управления буфером
    std::vector<char> buffer(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), buffer.data(), size_needed, NULL, NULL);

    // Возвращаем результат как std::string
    return std::string(buffer.begin(), buffer.end());
}

// Function for conversion UTF-8 → UTF-16 (std::string → std::wstring)
std::wstring utf8_to_wstring(const std::string& utf8_str) {
    // Определяем необходимый размер буфера для строки UTF-16
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    if (size_needed == 0) {
        return L""; // Если преобразование не удалось, возвращаем пустую строку
    }

    // Используем vector для автоматического управления буфером
    std::vector<wchar_t> buffer(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, buffer.data(), size_needed);

    // Возвращаем результат как std::wstring
    return std::wstring(buffer.begin(), buffer.end() - 1);  // исключаем завершающий нулевой символ
}
