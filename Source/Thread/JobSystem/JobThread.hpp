// Copyright 2020, Nathan Blane

#pragma once

#include <thread>
#include <condition_variable>

#include "BasicTypes/ConcurrentTypes.hpp"
#include "BasicTypes/Uncopyable.hpp"
#include "Thread/JobSystem/Job.hpp"
#include "Thread/ThreadingUtilities.hpp"

class JobSystem;

class JobThread : private Uncopyable
{
public:
	JobThread(JobSystem& jobSys, u32 index/*, ThreadSleepPrimitive& systemSleepPrim*/);
	~JobThread();

	void Start();
	void WaitStop();

	void WakeUp();

	JobQueue& GetWorkQueue() const;

private:
	void ThreadLoop();
	bool FindAndExecuteJob();

private:

	// TODO - Should probably use native threads...
	std::thread jobThread;
// 	ThreadSleepPrimitive jobNotify;
// 	ThreadSleepPrimitive& systemNotifier;
	JobSystem& jobSystem;
	JobQueue* jobQueue = nullptr;
	uflag threadRunning = ATOMIC_FLAG_INIT;
	u32 threadIndex;
};
