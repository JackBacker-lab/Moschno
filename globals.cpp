#include "globals.h"


std::atomic<bool> isKeyLoggerRunning(false);
bool isConversationRunning = false;
const int64_t sashaId = 1041837892;

/*
If the current mode is STANDARD, it means that this is a normal mode:
we can write commands, but if it is not STANDARD, it means that
the program will not accept commands, but will accept what
the mode is set to (path to folder, flags for executing subtasks, etc.)
*/

const std::string klFileName = "kltemp.txt";
const std::string cdFileName = "cdtemp.txt";
const std::string audioFileName = "output.raw";
Mode currentMode = Mode::STANDARD;
bool isWaitingForFile = false;
bool isMusicPlaying = false;

size_t totalFileNumber = 0;
size_t totalFileNameLength = 0;
uint64_t totalFileSize = 0;

std::string tempPath;

void initTempPath() {
    char buffer[MAX_PATH];
    GetTempPathA(MAX_PATH, buffer);
    tempPath = std::string(buffer);
}
