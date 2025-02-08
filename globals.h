#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>

extern const char* scrname;
extern const char* filename;
extern bool isKeyLoggerRunning;
extern bool isConversationRunning;
extern const int64_t sashaId;

enum Modes {
	MODE_STANDARD,
	MODE_CHECK_DIR,
	MODE_START_FILE,
	MODE_DELETE_FILE,
	MODE_COPY_FILE,
	MODE_SEND_FILE,
	MODE_UPLOAD_FILE,
	MODE_CONVERSATION,
};

extern int currentMode;
extern std::string conversationMode;
extern bool isWaitingForSecondPath;
extern bool isWaitingForFile;

#endif