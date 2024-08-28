// Copyright 2022, Nathan Blane

#include "Debugging/DebugOutput.hpp"
#include "DebugOutputWindowSink.hpp"
#include "Logging/LogLineEntry.hpp"

static fmt::memory_buffer logLineEntryBuffer;

void DebugOutputWindowSink::OutputFormattedString(const LogLineEntry& entry)
{
	constexpr size_t outputLen = 512;
	tchar outputStr[outputLen] = {};

	fmt::format_to(std::back_inserter(logLineEntryBuffer), "[TODO - TimeSinceBegin][{:s}]({:.{}}):{:s}\n",
		ToString(entry.level), entry.logSlot, Strlen(entry.logSlot), *entry.logMsg);

	size_t len = logLineEntryBuffer.size() >= outputLen ? outputLen - 1 : logLineEntryBuffer.size();
	Memory::Memcpy(outputStr, logLineEntryBuffer.data(), len);

	Debug::Print(outputStr);

	logLineEntryBuffer.clear();
}
