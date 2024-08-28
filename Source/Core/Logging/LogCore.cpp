
#include "LogCore.hpp"
#include "LoggingThread.hpp"
#include "File/DirectoryLocations.hpp"
#include "Sinks/ConsoleWindowSink.hpp"
#include "Sinks/DebugOutputWindowSink.hpp"
#include "Sinks/LogFileSink.hpp"

Logger& GetLogger()
{
	static Logger logger;
	return logger;
}

void Logger::InitLogging(LogLevel::Type level)
{
	loggingThread = new LoggingThread();
	logLevel = level;
}

void Logger::AddLogSink(LogSink* sink)
{
	Assert(loggingThread);
	Assert(sink);
	loggingThread->AddLogSink(*sink);
}

void Logger::RemoveLogSink(LogSink* sink)
{
	loggingThread->RemoveSink(*sink);
}

void Logger::PushLineToLog(const LogChannel& logChannel, LogLevel::Type level, const tchar* msg, size_t msgSize)
{
	// TODO - need to figure out if I don't format here or if I format here and then later not format
	LogLineEntry entry;
	entry.logMsg = String(msg, (u32)msgSize);
	entry.level = level;
	entry.logSlot = logChannel.logName;

	loggingThread->PushLogLine(entry);
}
