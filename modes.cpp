#include "modes.h"
#include "globals.h"


void checkDir(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	string path = message->text;
	wstring wpath = utf8_to_wstring(path);

	if (fs::exists(wpath) && fs::is_directory(wpath)) {
		string response;

		for (const auto& entry : fs::directory_iterator(wpath)) {
			wstring filePath = entry.path().wstring();
			string filePathUtf8 = wstringToUtf8(filePath);

			response += filePathUtf8 + "\n";
		}

		if (response.empty()) response = "Directory is empty.";
		bot.getApi().sendMessage(message->chat->id, response);
	}
	else {
		bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist or is not a directory.");
	}
}



void startFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

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


void deleteFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

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


void copyFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

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


void sendFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

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


void uploadFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	static TgBot::File::Ptr file;
	static string receivedFileContent;
	static string receivedFileName;

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
			catch (const exception& e) {
				bot.getApi().sendMessage(message->chat->id, std::string("Error receiving file: ") + e.what());
			}
		}
		else {
			bot.getApi().sendMessage(message->chat->id, "Please send a file first.");
		}
	}
	else {
		string path = message->text;
		wstring wpath = utf8_to_wstring(path);

		if (fs::exists(wpath) && fs::is_directory(wpath)) {
			wstring fullPath = wpath + L"\\" + utf8_to_wstring(receivedFileName);

			ofstream outFile(fullPath, ios::binary);

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



void listenMode(const TgBot::Bot& bot, const int64_t chat_id) {
	using namespace std;

	while (isConversationRunning) {
		try {
			string input;
			getline(cin, input);

			if (input.empty()) {
				bot.getApi().sendMessage(chat_id, "[ENTER]");
				continue;
			}
			bot.getApi().sendMessage(chat_id, input);
		}
		catch (const std::exception& e) {
			bot.getApi().sendMessage(chat_id, string("Error: ") + e.what());
		}
	}
}


void startConversation(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	using namespace std;
	try {
		cout << message->text << endl;
	}
	catch (const std::exception& e) {
		bot.getApi().sendMessage(message->chat->id, string("Error: ") + e.what());
	}
}