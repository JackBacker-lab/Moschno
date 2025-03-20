#include "modes.h"
#include "globals.h"
#include "conversions.h"
#include "handle_result.h"

// Auxiliary function for sendFile
static bool containsCyrillic(const std::string& str) {
	for (unsigned char c : str) {
		if ((c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8) {
			return true; // Approximate range of UTF-8 codes for Cyrillic characters
		}
	}
	return false;
}


// Auxiliary function for fullCheckDir
static Result scanDirectory(const std::wstring& rootPath, std::ofstream& outfile) {
	using namespace std;
	namespace fs = std::filesystem;
	COE code = COE::Success;
	string details = "";
	short count = 0;
	try {
		outfile << wstringToUtf8(rootPath) << "\\\n";

		for (const auto& entry : fs::directory_iterator(rootPath, fs::directory_options::skip_permission_denied)) {
			wstring itemName = entry.path().filename();
			outfile << "\t" << wstringToUtf8(itemName);

			if (entry.is_directory())
				outfile << "\\";
			outfile << "\n";

			if (++count == 100) {
				this_thread::sleep_for(chrono::milliseconds(10));
				this_thread::yield();
				count = 0;
			}
		}

		for (const auto& entry : fs::directory_iterator(rootPath, fs::directory_options::skip_permission_denied)) {
			if (entry.is_directory())
				scanDirectory(entry.path(), outfile);
		}
	}
	catch (const fs::filesystem_error& e) {
		code = COE::FilesystemError;
		details = string(e.what());
	}
	catch (const exception& e) {
		code = COE::UnexpectedError;
		details = string(e.what());
	}
	catch (...) {
		code = COE::UnknownError;
	}
	return { code, details, ResponseType::None, "" };
}


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


Result fullCheckDir(const std::string& message_text, const TgBot::Bot& bot, int64_t chatId) {
	using namespace std;
	namespace fs = std::filesystem;

	if (message_text.empty())
		return { COE::EmptyInput, "", ResponseType::None, "" };

	wstring wpath;
	try {
		wpath = utf8_to_wstring(message_text);
	}
	catch (const exception& e) {
		return { COE::ConversionError, e.what(), ResponseType::None, "" };
	}

	if (!fs::exists(wpath))
		return { COE::PathNotFound, "", ResponseType::None, "" };
	if (!fs::is_directory(wpath))
		return { COE::NotADirectory, "", ResponseType::None, "" };

	string tempFilePath = tempPath + cdFileName;

	ofstream tempFile(tempFilePath);
	if (!tempFile)
		return { COE::OpenFileError, "", ResponseType::None, "" };
	Result result = scanDirectory(wpath, tempFile);
	handleResult(result, bot, chatId);

	return { COE::Success, "", ResponseType::Path, tempFilePath };
}


// Old fullCheckDir
/*
Result fullCheckDir(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;

	if (message_text.empty())
		return { COE::EmptyInput, "", ResponseType::None, "" };

	wstring wpath;
	try {
		wpath = utf8_to_wstring(message_text);
	}
	catch (const exception& e) {
		return { COE::ConversionError, e.what(), ResponseType::None, "" };
	}

	if (!fs::exists(wpath))
		return { COE::PathNotFound, "", ResponseType::None, "" };
	if (!fs::is_directory(wpath))
		return { COE::NotADirectory, "", ResponseType::None, "" };

	unsigned long totalFilesNameLength = 0;
	unsigned int totalFilesNumber = 0;
	unsigned long long totalFilesSize = 0;

	try {
		char tempPath[MAX_PATH];
		GetTempPathA(MAX_PATH, tempPath);
		string tempFile = string(tempPath) + "cdtemp.txt";

		ofstream outfile(tempFile, ios::trunc);
		if (!outfile)
			return { COE::OpenFileError, "", ResponseType::None, "" };

		vector<wstring> filePaths;

		for (const auto& entry : fs::recursive_directory_iterator(wpath, fs::directory_options::skip_permission_denied)) {
			const auto& path = entry.path();
			const wstring& filePath = path.wstring();
			int fileNameLength = path.filename().wstring().length();
			totalFilesNameLength += fileNameLength;

			bool isFile = fs::is_regular_file(path);
			if (isFile) {
				totalFilesSize += fs::file_size(path);
				++totalFilesNumber;
			}

			filePaths.push_back(filePath);
		}

		ostringstream buffer;
		for (const auto& file : filePaths) {
			buffer << wstringToUtf8(file) << "\n";
		}

		double averageLength = (totalFilesNumber > 0) ? static_cast<double>(totalFilesNameLength) / totalFilesNumber : 0;
		double averageSize = (totalFilesNumber > 0) ? static_cast<double>(totalFilesSize) / totalFilesNumber : 0;

		buffer << "\n"
			<< "Total filename length: " << totalFilesNameLength << " symbols\n"
			<< "Total files number: " << totalFilesNumber << "\n"
			<< "Total files size: " << totalFilesSize << " bytes\n"
			<< "Average filename length: " << averageLength << "\n"
			<< "Average file size: " << averageSize << " bytes\n";

		outfile << buffer.str();
		outfile.close();

		return { COE::Success, "", ResponseType::Path, tempFile };
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
*/


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
		
		return { COE::Success, "", ResponseType::Text, "File has been successfully deleted: " + wstringToUtf8(wpath)};
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

	static wstring sourceFilePath;
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

			if (!fs::exists(sourceFilePath))
				return { COE::PathNotFound, "Source file does not exist.", ResponseType::None, "" };

			if (!fs::exists(fs::path(wpath).parent_path()))
				return { COE::PathNotFound, "Destination file directory does not exist.", ResponseType::None, "" };

			try {
				fs::copy(sourceFilePath, wpath, fs::copy_options::overwrite_existing);
				return { COE::Success, "", ResponseType::Text, "File has been successfully copied." };
			}
			catch (const fs::filesystem_error& e) {
				return { COE::FilesystemError, e.what(), ResponseType::None, "" };
			}
		}
		else {
			sourceFilePath = wpath;
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

	constexpr size_t MAX_TELEGRAM_FILE_SIZE = 20 * 1024 * 1024; // 20MB for TgBot
	bool isTempFile = false;

	string path = message_text;
	wstring wpath;
	try {
		wpath = utf8_to_wstring(path);
	}
	catch (const exception& e) {
		return { COE::ConversionError, e.what(), ResponseType::None, "" };
	}

	if (!fs::exists(wpath))
		return { COE::PathNotFound, "", ResponseType::None, "" };

	if (!fs::is_regular_file(wpath))
		return { COE::NotARegularFile, "", ResponseType::None, "" };

	try {
		if (containsCyrillic(path)) {
			string extension = fs::path(wpath).extension().string();
			string tempFilePath = string(tempPath) + "tempfile" + extension;

			fs::copy(wpath, tempFilePath, fs::copy_options::overwrite_existing);
			path = tempFilePath;
			isTempFile = true;
		}

		size_t fileSize = fs::file_size(path);

		if (fileSize <= MAX_TELEGRAM_FILE_SIZE) {
			bot.getApi().sendDocument(chatId, TgBot::InputFile::fromFile(path, "application/octet-stream"));
			if (isTempFile) {
				Result result = deleteFile(path);
				handleResult(result, bot, chatId);
			}
			return { COE::Success, "", ResponseType::Text, "File has been sent successfully." };
		}

		ifstream inputFile(path, ios::binary);
		if (!inputFile) {
			return { COE::FilesystemError, "Failed to open file for reading", ResponseType::None, "" };
		}

		vector<char> buffer(MAX_TELEGRAM_FILE_SIZE);
		size_t partNumber = 1;
		string baseName = fs::path(path).filename().string();

		while (inputFile) {
			string partFilename = baseName + ".part" + to_string(partNumber);
			ofstream outputFile(partFilename, ios::binary);
			if (!outputFile) {
				return { COE::FilesystemError, "Failed to create file part", ResponseType::None, "" };
			}

			inputFile.read(buffer.data(), MAX_TELEGRAM_FILE_SIZE);
			size_t bytesRead = inputFile.gcount();
			outputFile.write(buffer.data(), bytesRead);
			outputFile.close();

			bot.getApi().sendDocument(chatId, TgBot::InputFile::fromFile(partFilename, "application/octet-stream"));

			Result result = deleteFile(partFilename);
			handleResult(result, bot, chatId);

			partNumber++;
		}

		return { COE::Success, "", ResponseType::Text, "Large file sent in multiple parts." };
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
/*
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
*/


Result playMusic(const std::string& message_text) {
	namespace fs = std::filesystem;
	using namespace std;

	string path = message_text;
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