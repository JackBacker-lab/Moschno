#ifndef FILESYSTEM_FUNC_H
#define FILESYSTEM_FUNC_H

#include <iostream>
#include <tgbot/tgbot.h>
#include <filesystem>
#include <Shellapi.h>
#include "conversions.h"
#include <conio.h>

// Filesystem
void checkDir(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
void startFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
void deleteFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
void copyFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
void sendFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);
void uploadFile(const TgBot::Bot& bot, TgBot::Message::Ptr& message);

// Console conversation
void listenMode(const TgBot::Bot& bot, const int64_t chat_id);
void startConversation(const TgBot::Bot& bot, TgBot::Message::Ptr& message);

#endif