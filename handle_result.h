#ifndef HANDLE_RESULT_H
#define HANDLE_RESULT_H

#include <iostream>
#include <tgbot/tgbot.h>
#include "globals.h"
#include "conversions.h"
#include "modes.h"

void handleResult(Result& result, const TgBot::Bot& bot, int64_t chatId);

#endif