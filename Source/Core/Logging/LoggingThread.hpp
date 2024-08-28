#pragma once

#include "Threading/IThreadExecution.hpp"
#include "Threading/NativeThread.hpp"
#include "Threading/ScopedLock.hpp"
#include "Threading/Containers/LockingQueue.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Logging/LogSink.hpp"
#include "Logging/LogLineEntry.hpp"
#include "Time/Timer.h"
#include "File/FileSystem.hpp"

class Semaphore;

// Should own all structures that contain their portions to write to. 
// This should be all of the log sinks are put here instead of the logger. 
// Logger just funnels messages to this logging body
class LoggingThread : IThreadExecution
{
public:
	LoggingThread();
	~LoggingThread();

	void AddLogSink(LogSink& sink);
	void RemoveSink(LogSink& sink);

	void PushLogLine(const LogLineEntry& logLine);

private:
	// Thread methods
	virtual void ThreadInit() override;
	virtual void ThreadBody() override;
	virtual void ThreadExit() override;
	virtual void RequestStop() override;

private:
	CriticalSection sinksCriticalSection;
	DynamicArray<LogSink*> logOutputSinks;

	LockingQueue<LogLineEntry> entriesToProcess;

	DynamicArray<LogLineEntry> logLineEntries;
	NativeThread* logThread;

	Semaphore* linePushedSemaphore;

	Timer loggingTimings;

	FileSystem::Handle logFileHandle;

	// TODO - This needs to be an atomic type of some sort
	volatile bool stopRequested = false;
};
