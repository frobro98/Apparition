// Copyright 2020, Nathan Blane

#include "JobSystem.hpp"
#include "JobThread.hpp"
#include "Math/MathFunctions.hpp"

namespace
{
static JobSystem* jobSystem = new JobSystem;

thread_local u32 threadIndex;

using NotifyLock = std::unique_lock<std::mutex>;
}

void JobSystem::Initialize()
{
	u32 numThreads = 1;
	//numThreads = Math::Max(std::thread::hardware_concurrency() - 1, numThreads);
	//Assert(numThreads > 1);

	jobQueues.Resize(numThreads + 1);
	jobQueues[0] = new JobQueue(GetCurrentThreadID());
	threadIndex = 0;

	jobThreads.Resize(numThreads);
	u32 index = 1;
	for (auto& jobThread : jobThreads)
	{
		//jobThread = new JobThread(*this, index, systemSleepPrim);
		++index;
	}
}

void JobSystem::StartSystem()
{
	for (u32 i = 0; i < jobThreads.Size(); ++i)
	{
		jobThreads[i]->Start();
	}

	{
		//NotifyLock lock(systemSleepPrim.mut);
		//systemSleepPrim.condVar.notify_all();
	}
}

void JobSystem::Deinitialize()
{
	for (u32 i = 0; i < jobThreads.Size(); ++i)
	{
		jobThreads[i]->WaitStop();
		delete jobThreads[i];
	}

	jobThreads.Clear();
	jobQueues.Clear();
}

void JobSystem::SetJobQueueForIndex(JobQueue& q, u32 index)
{
	jobQueues[index] = &q;
	threadIndex = index;
}

JobQueue& JobSystem::GetRandomQueue(u32 index) const
{
	u32 randIndex;
	do
	{
		randIndex = rand() % jobQueues.Size();
	} while (randIndex == index);


	Assert(jobQueues[randIndex] != nullptr);
	return *jobQueues[randIndex];
}

Job* JobSystem::CreateEmptyJob() const
{
	return new Job();
}

Job* JobSystem::CreateJob(JobTask&& func) const
{
	return new Job(std::move(func), 0);
}

Job* JobSystem::CreateJobChild(Job& parent, JobTask&& func) const
{
	Job* j = new Job(std::move(func), 0, parent);
	parent.unfinishedChildren.fetch_add(1, std::memory_order_relaxed);
	return j;
}

void JobSystem::PushJob(Job& job)
{
	Assert(threadIndex < jobQueues.Size());
	
	while (!jobQueues[threadIndex]->TryPush(job));
}

void JobSystem::WaitOnJob(const Job& job)
{
	while (job.unfinishedChildren > 0)
	{
		JobQueue& jobQueue = *jobQueues[threadIndex];
		Job* foundJob = nullptr;
		// Do work from your own queue
		if (jobQueue.TryPop(foundJob))
		{
			ExecuteJob(*foundJob);
		}
		else
		{
			// Steal work from another queue
			JobQueue& queue = GetRandomQueue(0);
			if (queue.TrySteal(foundJob))
			{
				ExecuteJob(*foundJob);
			}
		}

		delete foundJob;
	}
}

void JobSystem::EndOfFrame()
{
	for (auto& queue : jobQueues)
	{
		queue->PrepareQueueForJobs();
	}
}

JobSystem& GetJobSystem()
{
	return *jobSystem;
}
