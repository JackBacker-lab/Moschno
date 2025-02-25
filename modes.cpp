#include "modes.h"
#include "globals.h"
#include "conversions.h"


void checkDir(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	if (!message || !message->text.length()) {
		bot.getApi().sendMessage(message->chat->id, "Invalid input: empty or null message.");
		return;
	}

	string path = message->text;
	wstring wpath;

	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, "Error converting path to wide string: " + string(e.what()));
		return;
	}

	if (!fs::exists(wpath)) {
		bot.getApi().sendMessage(message->chat->id, "Path does not exist.");
		return;
	}

	if (!fs::is_directory(wpath)) {
		bot.getApi().sendMessage(message->chat->id, "The path is not a directory.");
		return;
	}

	string response;
	size_t maxMessageSize = 4096; // Telegram API limit
	size_t currentSize = 0;

	try {
		for (const auto& entry : fs::directory_iterator(wpath)) {
			if (!entry.exists()) continue;

			wstring filePath = entry.path().wstring();
			string filePathUtf8 = wstringToUtf8(filePath);

			if (currentSize + filePathUtf8.length() + 1 > maxMessageSize) {
				bot.getApi().sendMessage(message->chat->id, response);
				response.clear();
				currentSize = 0;
			}

			response += filePathUtf8 + "\n";
			currentSize += filePathUtf8.length() + 1;
		}

		if (response.empty()) {
			bot.getApi().sendMessage(message->chat->id, "Directory is empty.");
		}
		else {
			bot.getApi().sendMessage(message->chat->id, response);
		}
	}
	catch (const fs::filesystem_error& e) {
		bot.getApi().sendMessage(message->chat->id, "Filesystem error: " + string(e.what()));
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, "Unexpected error: " + string(e.what()));
	}
	catch (...) {
		bot.getApi().sendMessage(message->chat->id, "Unknown error occurred.");
	}
}


void fullCheckDir(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	if (!message || message->text.empty()) {
		bot.getApi().sendMessage(message->chat->id, "Invalid input: empty or null message.");
		return;
	}

	string path = message->text;
	wstring wpath;

	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, "Error converting path to wide string: " + string(e.what()));
		return;
	}

	if (!fs::exists(wpath)) {
		bot.getApi().sendMessage(message->chat->id, "Path does not exist.");
		return;
	}

	if (!fs::is_directory(wpath)) {
		bot.getApi().sendMessage(message->chat->id, "The path is not a directory.");
		return;
	}

	unsigned long totalFilesNameLength = 0;
	unsigned int totalFilesNumber = 0;
	unsigned long long totalFilesSize = 0;

	try {
		ofstream outfile(cdfilename, ios::trunc);
		if (!outfile) {
			bot.getApi().sendMessage(message->chat->id, "Error: Unable to open file for writing.");
			return;
		}

		unsigned long totalFilesNameLength = 0;
		unsigned int totalFilesNumber = 0;
		unsigned long long totalFilesSize = 0;

		for (const auto& entry : fs::recursive_directory_iterator(wpath, fs::directory_options::skip_permission_denied)) {
			wstring filePath = entry.path().wstring();
			int fileNameLength = entry.path().filename().wstring().length();
			totalFilesNameLength += fileNameLength;

			if (fs::is_regular_file(entry.path())) {
				totalFilesSize += fs::file_size(entry.path());
				++totalFilesNumber;
			}

			outfile << wstringToUtf8(filePath) << "\n";
		}

		double averageLength = (totalFilesNumber > 0) ? static_cast<double>(totalFilesNameLength) / totalFilesNumber : 0;
		double averageSize = (totalFilesNumber > 0) ? static_cast<double>(totalFilesSize) / totalFilesNumber : 0;

		outfile << "\n"
			<< "Total filename length: " << totalFilesNameLength << " symbols\n"
			<< "Total files number: " << totalFilesNumber << "\n"
			<< "Total files size: " << totalFilesSize << " bytes\n"
			<< "Average filename length: " << averageLength << "\n"
			<< "Average file size: " << averageSize << " bytes\n";

		bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(cdfilename, "text/plain"));
	}
	catch (const fs::filesystem_error& e) {
		bot.getApi().sendMessage(message->chat->id, "Filesystem error: " + string(e.what()));
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, "Unexpected error: " + string(e.what()));
	}
	catch (...) {
		bot.getApi().sendMessage(message->chat->id, "Unknown error occurred.");
	}
}


void startFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	try {
		if (!message || message->text.empty()) {
			bot.getApi().sendMessage(message->chat->id, "Invalid input: empty or null message.");
			return;
		}

		string path = message->text;
		wstring wpath;

		try {
			wpath = utf8_to_wstring(path);
		}
		catch (const exception& e) {
			bot.getApi().sendMessage(message->chat->id, "Error converting path to wide string: " + string(e.what()));
			return;
		}

		if (!fs::exists(wpath)) {
			bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist.");
			return;
		}

		// Execute file/program
		HINSTANCE result = ShellExecuteW(NULL, L"open", wpath.c_str(), NULL, NULL, SW_SHOWNORMAL);

		if ((intptr_t)result <= 32) {
			string errorMsg;
			switch ((intptr_t)result) {
			case SE_ERR_FNF: errorMsg = "File not found."; break;
			case SE_ERR_PNF: errorMsg = "Path not found."; break;
			case SE_ERR_ACCESSDENIED: errorMsg = "Access denied."; break;
			case SE_ERR_OOM: errorMsg = "Out of memory."; break;
			default: errorMsg = "Unknown execution error.";
			}
			bot.getApi().sendMessage(message->chat->id, "Error executing program: " + errorMsg);
		}
		else {
			bot.getApi().sendMessage(message->chat->id, "Program executed successfully.");
		}
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, "Unexpected error: " + string(e.what()));
	}
	catch (...) {
		bot.getApi().sendMessage(message->chat->id, "Unknown error occurred.");
	}
}


void deleteFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;
	try {
		string path = message->text;
		wstring wpath;

		try {
			wpath = utf8_to_wstring(path);
		}
		catch (const exception& e) {
			bot.getApi().sendMessage(message->chat->id, "Error converting path to wide string: " + string(e.what()));
			return;
		}

		if (!fs::exists(wpath)) {
			bot.getApi().sendMessage(message->chat->id, "Path does not exist.");
			return;
		}

		if (!fs::is_regular_file(wpath)) {
			bot.getApi().sendMessage(message->chat->id, "The path is not a regular file.");
			return;
		}

		if (!fs::remove(wpath)) {
			bot.getApi().sendMessage(message->chat->id, "Cannot delete the file.");
			return;
		}
		
		bot.getApi().sendMessage(message->chat->id, "File has been successfully deleted.");
	}
	catch (const fs::filesystem_error& e) {
		bot.getApi().sendMessage(message->chat->id, "Filesystem error: " + string(e.what()));
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, "Unexpected error: " + string(e.what()));
	}
	catch (...) {
		bot.getApi().sendMessage(message->chat->id, "Unknown error occurred.");
	}
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

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	while (isConversationRunning) {
		try {
			cin.clear();
			wstring winput;
			getline(wcin, winput);

			if (winput.empty()) {
				bot.getApi().sendMessage(chat_id, "[ENTER]");
				continue;
			}

			string input = wstringToUtf8(winput);
			bot.getApi().sendMessage(chat_id, input);
		}
		catch (const std::exception& e) {
			bot.getApi().sendMessage(chat_id, string("Error: ") + e.what());
		}
	}
}


void startConversation(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	using namespace std;

	SetConsoleOutputCP(CP_UTF8);

	try {
		cout << message->text << endl;
		bot.getApi().sendMessage(message->chat->id, "Your message had successfully sended.");
	}
	catch (const std::exception& e) {
		bot.getApi().sendMessage(message->chat->id, string("Error: ") + e.what());
	}
}


void playMusic(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	string path = message->text;
	wstring wpath = utf8_to_wstring(path);

	if (!fs::exists(wpath)) {
		bot.getApi().sendMessage(message->chat->id, "Error: File does not exist.");
		return;
	}

	ifstream file(wpath);
	if (!file.is_open()) {
		bot.getApi().sendMessage(message->chat->id, "Error: Cannot open file.");
		return;
	}
	file.close();

	try {
		mciSendString((L"open \"" + wpath + L"\" type mpegvideo alias myMP3").c_str(), NULL, 0, NULL);
		mciSendString(L"play myMP3", NULL, 0, NULL);
		isMusicPlaying = true;
		bot.getApi().sendMessage(message->chat->id, "Music is playing.");
	}
	catch (const std::exception& e) {
		bot.getApi().sendMessage(message->chat->id, string("Error: ") + e.what());
	}
}