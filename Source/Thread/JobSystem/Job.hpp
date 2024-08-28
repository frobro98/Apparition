// Copyright 2020, Nathan Blane

#pragma once

#include <functional>

#include "BasicTypes/Intrinsics.hpp"
#include "Debugging/Assertion.hpp"
#include "Thread/JobSystem/StealingConcurrentDeque.hpp"

using JobTask = std::function<void()>;
using JobQueue = WorkStealingDeque<512>;

struct Job
{
	JobTask task;
	Job* parent = nullptr;
	uatom32 unfinishedChildren{ 0 };
	const u32 jobID = 0;

	i32 pad[1] = { 0 };

	Job() = default;

	template<class Func>
	Job(Func&& f, u32 id)
		: task(std::forward<Func>(f)),
		jobID(id)
	{
	}

	template<class Func>
	Job(Func&& f, u32 id, Job& par)
		: task(std::forward<Func>(f)),
		parent(&par),
		jobID(id)
	{
	}

	void Perform()
	{
		Assert(task);
		task();
	}
};

void ExecuteJob(Job& job);
