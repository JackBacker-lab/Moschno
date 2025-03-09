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
Result checkDir(std::string message_text);
Result fullCheckDir(std::string message_text);
Result startFile(std::string message_text);
Result deleteFile(std::string message_text);
Result copyFile(std::string message_text);
void sendFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
void uploadFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);

// Console conversation
void listenMode(const TgBot::Bot& bot, const int64_t chat_id);
void startConversation(const TgBot::Bot& bot, TgBot::Message::Ptr& message);

// Music
void playMusic(const TgBot::Bot& bot, TgBot::Message::Ptr& message);

#endif