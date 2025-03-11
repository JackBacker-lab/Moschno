#include "modes.h"
#include "globals.h"
#include "conversions.h"


Result checkDir(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;

	if (!message_text.length()) 
		return { COE::EmptyInput, "", ResponseType::None, ""};

	string path = message_text;
	wstring wpath;

	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) { 
		return { COE::ConversionError, e.what(), ResponseType::None, "" }; }

	if (!fs::exists(wpath)) 
		return { COE::PathNotFound, "", ResponseType::None, "" };

	if (!fs::is_directory(wpath)) 
		return { COE::NotADirectory, "", ResponseType::None, "" };

	string response;
	size_t maxMessageSize = 4096; // Telegram API limit
	size_t currentSize = 0;

	try {
		for (const auto& entry : fs::directory_iterator(wpath)) {
			if (!entry.exists()) continue;

			wstring filePath = entry.path().wstring();
			string filePathUtf8 = wstringToUtf8(filePath);

			if (currentSize + filePathUtf8.length() + 1 > maxMessageSize) 
				return { COE::LimitError, "Tip: use /full_check_dir_mode for this directory.", ResponseType::None, ""};

			response += filePathUtf8 + "\n";
			currentSize += filePathUtf8.length() + 1;
		}

		if (response.empty()) 
			return { COE::EmptyDirectory, "", ResponseType::None, "" };

		return { COE::Success, "", ResponseType::Text, response};
	}
	catch (const fs::filesystem_error& e) {
		return { COE::FilesystemError, string(e.what()), ResponseType::None, ""};
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, string(e.what()), ResponseType::None, ""};
	}
	catch (...) {
		return { COE::UnknownError, "", ResponseType::None, "" };
	}
}


Result fullCheckDir(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;

	if (!message_text.length()) 
		return { COE::EmptyInput, "", ResponseType::None, "" };

	string path = message_text;
	wstring wpath;

	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) { 
		return { COE::ConversionError, e.what(), ResponseType::None, "" }; }

	if (!fs::exists(wpath)) 
		return { COE::PathNotFound, "", ResponseType::None, "" };

	if (!fs::is_directory(wpath)) 
		return { COE::NotADirectory, "", ResponseType::None, "" };

	unsigned long totalFilesNameLength = 0;
	unsigned int totalFilesNumber = 0;
	unsigned long long totalFilesSize = 0;

	try {
		ofstream outfile(cdfilename, ios::trunc);
		if (!outfile) 
			return { COE::OpenFileError, "", ResponseType::None, "" };

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

		return { COE::Success, "", ResponseType::Path, cdfilename };
	}
	catch (const fs::filesystem_error& e) {
		return { COE::FilesystemError, string(e.what()), ResponseType::None, "" };
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, string(e.what()), ResponseType::None, "" };
	}
	catch (...) {
		return { COE::UnknownError, "", ResponseType::None, "" };
	}
}


Result startFile(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;

	try {
		if (message_text.empty()) 
			return { COE::EmptyInput, "", ResponseType::None, "" };

		string path = message_text;
		wstring wpath;

		try {
			wpath = utf8_to_wstring(path);
		}
		catch (const exception& e) { 
			return { COE::ConversionError, e.what(), ResponseType::None, "" }; };

		if (!fs::exists(wpath)) 
			return { COE::PathNotFound, "", ResponseType::None, "" };

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

			return { COE::ExecutionError, errorMsg, ResponseType::None, "" };
		}
		return { COE::Success, "", ResponseType::Text, "Program executed successfully."};
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, string(e.what()), ResponseType::None, "" };
	}
	catch (...) {
		return { COE::UnknownError, "", ResponseType::None, "" };
	}
}


Result deleteFile(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;
	try {
		string path = message_text;
		wstring wpath;

		try {
			wpath = utf8_to_wstring(path);
		}
		catch (const exception& e) { 
			return { COE::ConversionError, e.what(), ResponseType::None, "" }; };

		if (!fs::exists(wpath)) 
			return { COE::PathNotFound, "", ResponseType::None, "" };

		if (!fs::is_regular_file(wpath)) 
			return { COE::NotARegularFile, "", ResponseType::None, "" };

		if (!fs::remove(wpath)) 
			return { COE::RemoveFileError, "", ResponseType::None, "" };
		
		return { COE::Success, "", ResponseType::Text, "File has been successfully deleted." };
	}
	catch (const fs::filesystem_error& e) {
		return { COE::FilesystemError, string(e.what()), ResponseType::None, "" };
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, string(e.what()), ResponseType::None, "" };
	}
	catch (...) {
		return { COE::UnknownError, "", ResponseType::None, "" };
	}
}


Result copyFile(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;

	static wstring tempPath;
	static bool isWaitingForSecondPath = false;

	try {
		wstring wpath;
		try {
			wpath = utf8_to_wstring(message_text);
		}
		catch (const exception& e) {
			return { COE::ConversionError, e.what(), ResponseType::None, "" }; };

		if (isWaitingForSecondPath) {
			isWaitingForSecondPath = false;

			if (!fs::exists(tempPath))
				return { COE::PathNotFound, "Source file does not exist.", ResponseType::None, "" };

			if (!fs::exists(fs::path(wpath).parent_path()))
				return { COE::PathNotFound, "Destination file directory does not exist.", ResponseType::None, "" };

			try {
				fs::copy(tempPath, wpath, fs::copy_options::overwrite_existing);
				return { COE::Success, "", ResponseType::Text, "File has been successfully copied." };
			}
			catch (const fs::filesystem_error& e) {
				return { COE::FilesystemError, e.what(), ResponseType::None, "" };
			}
		}
		else {
			tempPath = wpath;
			isWaitingForSecondPath = true;
			return { COE::Success, "", ResponseType::Text, "Send a path where you want your file to be copied." };
		}
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, e.what(), ResponseType::None, "" };
	}
	catch (...) {
		return { COE::UnknownError, "An unknown error occurred.", ResponseType::None, "" };
	}
}


Result sendFile(const TgBot::Bot& bot, const std::string& message_text, int64_t chatId) {
	namespace fs = std::filesystem;
	using namespace std;

	string path = message_text;
	wstring wpath;
	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) {
		return { COE::ConversionError, e.what(), ResponseType::None, "" };
	};

	if (!fs::exists(wpath))
		return { COE::PathNotFound, "", ResponseType::None, "" };

	if (!fs::is_regular_file(wpath))
		return { COE::NotARegularFile, "", ResponseType::None, "" };

	try {
		string originalFileName = fs::path(wpath).filename().string();
		string extension = fs::path(wpath).extension().string();

		char tempPath[MAX_PATH];
		GetTempPathA(MAX_PATH, tempPath);
		string tempFile = string(tempPath) + "tempfile" + extension;

		fs::copy(wpath, tempFile, fs::copy_options::overwrite_existing);
		bot.getApi().sendDocument(chatId, TgBot::InputFile::fromFile(tempFile, "application/octet-stream"), originalFileName);

		fs::remove(tempFile);
		return { COE::Success, "", ResponseType::Text, "File has been sent successfully." };
	}
	catch (const fs::filesystem_error& e) {
		return { COE::FilesystemError, string(e.what()), ResponseType::None, "" };
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, string(e.what()), ResponseType::None, "" };
	}
	catch (...) {
		return { COE::UnknownError, "", ResponseType::None, "" };
	}
}


Result uploadFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	static TgBot::File::Ptr file;
	static string receivedFileContent;
	static string receivedFileName;

	if (isWaitingForFile) {
		if (!message->document)
			return { COE::NotAFile, "", ResponseType::None, ""};

		try {
			file = bot.getApi().getFile(message->document->fileId);
			receivedFileContent = bot.getApi().downloadFile(file->filePath);
			receivedFileName = message->document->fileName;

			isWaitingForFile = false;
			return { COE::Success, "", ResponseType::Text, "Send a path, where you want your file to be uploaded." };
		}
		catch (const exception& e) {
			return { COE::UnexpectedError, string(e.what()), ResponseType::None, "" };
		}
	}
	else {
		string path = message->text;
		wstring wpath;
		try {
			wpath = utf8_to_wstring(path);
		}
		catch (const exception& e) {
			return { COE::ConversionError, e.what(), ResponseType::None, "" };
		};

		if (!fs::exists(wpath))
			return { COE::PathNotFound, "", ResponseType::None, "" };

		if (!fs::is_directory(wpath))
			return { COE::NotADirectory, "", ResponseType::None, "" };
			
		wstring fullPath;
		try {
			fullPath = wpath + L"\\" + utf8_to_wstring(receivedFileName);
		}
		catch (const exception& e) {
			return { COE::ConversionError, e.what(), ResponseType::None, "" };
		};

		ofstream outFile(fullPath, ios::binary);

		if (!outFile.is_open()) 
			return { COE::OpenFileError, "", ResponseType::None, ""};

		outFile.write(receivedFileContent.data(), receivedFileContent.size());
		outFile.close();

		isWaitingForFile = true;
		return{ COE::Success, "", ResponseType::Text, "File has been uploaded successfully."};
	}
}


// Decommissioned
void listenMode(const TgBot::Bot& bot, int64_t chat_id) {
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
		catch (const exception& e) {
			bot.getApi().sendMessage(chat_id, string("Error: ") + e.what());
		}
	}
}


// Decommissioned
void startConversation(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	using namespace std;

	SetConsoleOutputCP(CP_UTF8);

	try {
		cout << message->text << endl;
		bot.getApi().sendMessage(message->chat->id, "Your message had successfully sended.");
	}
	catch (const exception& e) {
		bot.getApi().sendMessage(message->chat->id, string("Error: ") + e.what());
	}
}


Result playMusic(const TgBot::Bot& bot, TgBot::Message::Ptr& message) {
	namespace fs = std::filesystem;
	using namespace std;

	string path = message->text;
	wstring wpath;

	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) {
		return { COE::ConversionError, e.what(), ResponseType::None, "" };
	}

	if (!fs::exists(wpath))
		return { COE::PathNotFound, "", ResponseType::None, "" };

	ifstream file(wpath);

	if (!file.is_open())
		return { COE::OpenFileError, "", ResponseType::None, "" };

	file.close();

	try {
		mciSendString((L"open \"" + wpath + L"\" type mpegvideo alias myMP3").c_str(), NULL, 0, NULL);
		mciSendString(L"play myMP3", NULL, 0, NULL);
		isMusicPlaying = true;
		return { COE::Success, "", ResponseType::Text, "Music is playing." };
	}
	catch (const exception& e) {
		return { COE::UnexpectedError, string(e.what()), ResponseType::None, "" };
	}
	catch (...) {
		return { COE::UnknownError, "", ResponseType::None, "" };
	}
}