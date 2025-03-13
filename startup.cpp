#include "startup.h"

void AddToStartup(const std::wstring& appName, const std::wstring& exePath) {
	HKEY hKey;

	// Open the registry key
	if (RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS) {
		// Adding an entry to autostart
		if (RegSetValueExW(hKey, appName.c_str(), 0, REG_SZ,
			reinterpret_cast<const BYTE*>(exePath.c_str()),
			(exePath.size() + 1) * sizeof(wchar_t)) == ERROR_SUCCESS) {
			std::wcout << L"Program has been successfully added to startup with name: " << appName << std::endl;
		}
		else {
			std::wcerr << L"Error while adding program to startup!" << std::endl;
		}
		RegCloseKey(hKey);
	}
	else {
		std::wcerr << L"Cannot open the register key!" << std::endl;
	}
}