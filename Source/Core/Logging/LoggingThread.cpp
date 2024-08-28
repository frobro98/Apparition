#include "LoggingThread.hpp"
#include "Platform/PlatformThreading.hpp"
#include "Threading/Semaphore.hpp"
#include "BasicTypes/Limits.hpp"

LoggingThread::LoggingThread()
{
	linePushedSemaphore = new Semaphore(0, I32Max);
	// NOTE - These calls shouldn't be combined
	logThread = PlatformThreading::CreateThread();
	logThread->StartWithBody("Log Thread", ThreadPriority::High, *this);
}

LoggingThread::~LoggingThread()
{
	// TODO - If I am to cache OS sync events, I should push it back here
	RequestStop();
}

void LoggingThread::AddLogSink(LogSink& sink)
{
	Assert(!stopRequested);
	REF_CHECK(sink);
	ScopedLock sinkLock(sinksCriticalSection);
	logOutputSinks.Add(&sink);
}

void LoggingThread::RemoveSink(LogSink& sink)
{
	Assert(!stopRequested);
	REF_CHECK(sink);
	ScopedLock sinkLock(sinksCriticalSection);
	logOutputSinks.RemoveFirstOf(&sink);
}

void LoggingThread::PushLogLine(const LogLineEntry& logLine)
{
	entriesToProcess.Push(logLine);
	linePushedSemaphore->Signal();
}

void LoggingThread::ThreadInit()
{
	Assert(linePushedSemaphore);
	Assert(logThread);
}

void LoggingThread::ThreadBody()
{
	linePushedSemaphore->Wait();

	while (!stopRequested)
	{
		LogLineEntry entry;
		Assert(!entriesToProcess.IsEmpty());
		entry = entriesToProcess.Pop();

		Assert(!entry.logMsg.IsEmpty());
		{
			ScopedLock sinkLock(sinksCriticalSection);
			for (auto& logSink : logOutputSinks)
			{
				logSink->OutputFormattedString(entry);
			}
		}

		// TODO - Should keep a ring queue of line entries. Or the thread shouldn't even keep track of this shit anyways...
		//logLineEntries.Add(entry);
		
		linePushedSemaphore->Wait();
	}
}

void LoggingThread::ThreadExit()
{
	Assert(stopRequested);
}

void LoggingThread::RequestStop()
{
	// TODO - This needs to be an atomic operation...
	stopRequested = true;
	linePushedSemaphore->Signal();
}
