// Copyright 2022, Nathan Blane

#include "Platform/PlatformDefinitions.h"
#include "LogFileSink.hpp"
#include "Logging/LogLineEntry.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// TODO - Initial time stamp of file open, down to the millisecond
// Then after the time stamp, it uses the offset from that in milliseconds
static void FormatDateAndTime(fmt::memory_buffer& buff)
{
	// TODO - Windows Specific!!!
	SYSTEMTIME systemTime;
	// GetLocalTime gets the time with the time zone applied. 
	// GetSystemTime gets the time with the time zone NOT applied (UTC)
	::GetLocalTime(&systemTime);

	fmt::format_to(std::back_inserter(buff), "Musa log opened at {:02d}:{:02d}:{:02d}.{:03d} on {:02d}/{:02d}/{:04d}\n",
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds,
		systemTime.wMonth, systemTime.wDay, systemTime.wYear);
}

static fmt::memory_buffer logLineEntryBuffer;

LogFileSink::LogFileSink(const Path& filePath)
{
	bool result = FileSystem::OpenFile(logFileHandle, filePath.GetString(), FileMode::Write);
	Assert(result);

	fmt::memory_buffer logHeader;
	FormatDateAndTime(logHeader);
	FileSystem::WriteFile(logFileHandle, logHeader.data(), (u32)logHeader.size());
}

LogFileSink::~LogFileSink()
{
	const tchar* endLogText = "Log closed, application terminated\n";
	bool result = FileSystem::WriteFile(logFileHandle, endLogText, (u32)Strlen(endLogText));
	Assert(result);

	// TODO - Say when we closed it, just so we know when the thread terminated...
	result = FileSystem::CloseFile(logFileHandle);
	Assert(result);
}

void LogFileSink::OutputFormattedString(const LogLineEntry& entry)
{
	// TODO - Get time difference from the start of the program execution
		// Format goes: Time since start up, log level, log channel, log msg
	fmt::format_to(std::back_inserter(logLineEntryBuffer), "[TODO - TimeSinceBegin][{:s}]({:.{}}):{:s}\n",
		ToString(entry.level), entry.logSlot, Strlen(entry.logSlot), *entry.logMsg);

	NOT_USED bool result = FileSystem::WriteFile(logFileHandle, logLineEntryBuffer.data(), (u32)logLineEntryBuffer.size());
	Assert(result);

	logLineEntryBuffer.clear();
}
