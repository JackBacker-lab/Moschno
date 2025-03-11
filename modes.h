#ifndef FILESYSTEM_FUNC_H
#define FILESYSTEM_FUNC_H

#include <iostream>
#include <tgbot/tgbot.h>
#include <filesystem>
#include <Shellapi.h>
#include "conversions.h"
#include "globals.h"
#include <conio.h>
#include <mmsystem.h>
#include <queue>

#pragma comment(lib, "winmm.lib")

// Filesystem
Result checkDir(const std::string& message_text);
Result fullCheckDir(const std::string& message_text);
Result startFile(const std::string& message_text);
Result deleteFile(const std::string& message_text);
Result copyFile(const std::string& message_text);
Result sendFile(const TgBot::Bot& bot, const std::string& message_text, int64_t chatId);
Result uploadFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);

// Console conversation
/*
void listenMode(const TgBot::Bot& bot, int64_t chat_id);
void startConversation(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
*/

// Music
Result playMusic(const std::string& message);

#endif