// Copyright 2020, Nathan Blane

#include "Job.hpp"
#include "JobUtilities.hpp"

void ExecuteJob(Job& job)
{
	SetActiveJobParent(job);
	job.Perform();

	GetJobSystem().WaitOnJob(job);

	if (job.parent != nullptr)
	{
		job.parent->unfinishedChildren.fetch_sub(1, std::memory_order_relaxed);
	}
}
