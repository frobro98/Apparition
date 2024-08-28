
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ConsoleWindowSink.hpp"
#include "Logging/LogLineEntry.hpp"

// TODO - There is a memory_buffer for each sink so far. That isn't a good thing. Fix this!
static fmt::memory_buffer logLineEntryBuffer;

ConsoleWindowSink::ConsoleWindowSink()
	: consoleOutHandle(INVALID_HANDLE_VALUE)
{
	// TODO - Remove this stuff because after allocation, we write directly to the console...
	FILE* stdOut = nullptr;
	AllocConsole();
	freopen_s(&stdOut, "CONOUT$", "w", stdout);

	HWND console = GetConsoleWindow();
	RECT rect = {};
	GetWindowRect(console, &rect);
	SetWindowPos(console, console, rect.left + (rect.left / 2), rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);

	consoleOutHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);

	levelColors[LogLevel::Debug] = FOREGROUND_GREEN;
	levelColors[LogLevel::Info] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	levelColors[LogLevel::Warning] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	levelColors[LogLevel::Error] = FOREGROUND_RED;
	levelColors[LogLevel::Fatal] = FOREGROUND_RED | FOREGROUND_INTENSITY;
}

ConsoleWindowSink::~ConsoleWindowSink()
{
	FreeConsole();
	consoleOutHandle = INVALID_HANDLE_VALUE;
}

void ConsoleWindowSink::OutputFormattedString(const LogLineEntry& entry)
{
	Assert(consoleOutHandle != INVALID_HANDLE_VALUE);
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	GetConsoleScreenBufferInfo(consoleOutHandle, &sbInfo);
	WORD origAttributes = sbInfo.wAttributes;

	SetConsoleTextAttribute(consoleOutHandle, levelColors[entry.level]);

	fmt::format_to(std::back_inserter(logLineEntryBuffer), "[TODO - TimeSinceBegin][{:s}]({:.{}}):{:s}\n",
		ToString(entry.level), entry.logSlot, Strlen(entry.logSlot), *entry.logMsg);

	u32 charsWritten;
	WriteConsole(consoleOutHandle, logLineEntryBuffer.data(), (DWORD)logLineEntryBuffer.size(), (DWORD*)&charsWritten, nullptr);

	SetConsoleTextAttribute(consoleOutHandle, origAttributes);

	logLineEntryBuffer.clear();
}
