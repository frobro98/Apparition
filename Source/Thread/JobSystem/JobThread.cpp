// Copyright 2020, Nathan Blane

#include "JobThread.hpp"
#include "JobSystem.hpp"

namespace
{

using NotifyLock = std::unique_lock<std::mutex>;

}

JobThread::JobThread(JobSystem& jobSys, u32 index/*, ThreadSleepPrimitive& systemSleepPrim*/)
	: //systemNotifier(systemSleepPrim),
	jobSystem(jobSys),
	threadIndex(index)
{
}

JobThread::~JobThread()
{
	WaitStop();
}

void JobThread::Start()
{
	jobThread = std::thread(&JobThread::ThreadLoop, this);
}

void JobThread::WaitStop()
{
	if (jobThread.joinable())
	{
		threadRunning.clear(std::memory_order_seq_cst);
		WakeUp();
		jobThread.join();
	}
}

void JobThread::WakeUp()
{
// 	NotifyLock lock(jobNotify.mut);
// 	jobNotify.condVar.notify_one();
}

JobQueue& JobThread::GetWorkQueue() const
{
	Assert(jobQueue != nullptr);
	return *jobQueue;
}

void JobThread::ThreadLoop()
{
	jobQueue = new JobQueue(GetCurrentThreadID());
	jobSystem.SetJobQueueForIndex(*jobQueue, threadIndex);
	{
// 		NotifyLock lock(systemNotifier.mut);
// 		systemNotifier.condVar.wait(lock);
	}

	threadRunning.test_and_set(std::memory_order_relaxed);

	bool runningValue = false;
	do
	{
		while (FindAndExecuteJob())
		{
		}

		std::this_thread::yield();

		runningValue = threadRunning.test_and_set(std::memory_order_seq_cst);
	} while (runningValue);

	delete jobQueue;
}

bool JobThread::FindAndExecuteJob()
{
	Job* job = nullptr;
	bool foundValidJob = false;
	if (jobQueue->TryPop(job))
	{
		ExecuteJob(*job);
		foundValidJob = true;
	}
	else
	{
		JobQueue& queue = jobSystem.GetRandomQueue(threadIndex);
		if (queue.TrySteal(job))
		{
			ExecuteJob(*job);
			foundValidJob = true;
		}
	}

	delete job;
	return foundValidJob;
}
