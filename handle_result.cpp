#include "handle_result.h"


// Auxiliary function that analyzes and handles main funtions' results
void handleResult(Result& result, const TgBot::Bot& bot, int64_t chatId) {
	if (result.code == COE::Success) {
		switch (result.response_type) {
		case ResponseType::None:
			break;
		case ResponseType::Text:
			bot.getApi().sendMessage(chatId, result.response);
			break;
		case ResponseType::Path:
			result = sendFile(bot, result.response, chatId);
			handleResult(result, bot, chatId);
			if (currentMode == Mode::FULL_CHECK_DIR) {
				Result result = deleteFile(result.response);
				handleResult(result, bot, chatId);
			}
		}
		return;
	}

	std::string details = result.errorDetails;
	std::wstring wdetails = utf8_to_wstring(details);
	details = wstringToUtf8(wdetails);

	std::string message;
	switch (result.code) {
	case COE::EmptyInput:	   message = "Invalid input: empty or null message. "; break;

	case COE::EmptyDirectory:  message = "Directory is empty. "; break;

	case COE::PathNotFound:	   message = "Path does not exist. "; break;

	case COE::NotADirectory:   message = "The path is not a directory. "; break;

	case COE::ConversionError: message = "Error converting path to wide string. "; break;

	case COE::FilesystemError: message = "Filesystem error. "; break;

	case COE::UnexpectedError: message = "Unexpected error. "; break;

	case COE::ExecutionError:  message = "Execution error. "; break;

	case COE::UnknownError:	   message = "Unknown error occurred. "; break;

	case COE::OpenFileError:   message = "Error opening a file. "; break;

	case COE::LimitError:	   message = "Error: Telegram limit exceeded for message size. "; break;

	case COE::NotARegularFile: message = "The path is not a regular file. "; break;

	case COE::RemoveFileError: message = "Cannot delete the file. "; break;

	case COE::NotAFile:		   message = "Please send a file first. "; break;

	case COE::TakeScreenshotError: message = "Error while taking screenshot. "; break;

	case COE::RecordAudioError: message = "Error while recording audio. "; break;
	}
	bot.getApi().sendMessage(chatId, message + details);
}