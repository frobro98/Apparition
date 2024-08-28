// Copyright 2020, Nathan Blane

#pragma once

#include "Thread/JobSystem/Job.hpp"
#include "Thread/JobSystem/JobSystem.hpp"

void SetActiveJobParent(Job& job);
Job& GetActiveJobParent();

template <class Func>
void PushConcurrentJob(Func&& func)
{
	Job* j = GetJobSystem().CreateJob(std::move(func));
	GetJobSystem().PushJob(*j);
}

template <class Func>
void PushConcurrentJobUnder(Job& parent, Func&& func)
{
	Job* j = GetJobSystem().CreateJobChild(parent, std::move(func));
	GetJobSystem().PushJob(*j);
}