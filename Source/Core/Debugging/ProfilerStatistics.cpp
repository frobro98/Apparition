// Copyright 2020, Nathan Blane

#include "ProfilerStatistics.hpp"
#include "MetricInterface.hpp"
#include "Debugging/MetricInterface.hpp"
#include "Containers/Stack.hpp"

static u32 frameNumber = 0;

static ProfilerStatistics stats;

void ProfilerStatistics::ProfileFrameIncrement()
{
	GetMetricTable().SwapTablesForNewFrame();

	++frameNumber;
	currentFrameIndex = frameNumber % frameProfileCount;
}

void ProfilerStatistics::CollectAllFrameMetrics()
{
	// TODO - Collection of metrics should be also profiled. Issue is that if a scoped timed block is used, it will be in complete
	const MetricTable::TableEntries& entries = GetMetricTable().GetCurrentTable();

	ProfiledFrameMark& mark = frameProfiles[currentFrameIndex];
	mark.Clear();
	Stack<MetricEvent> eventStack(entries.Size() / 2);
	for (const auto& entry : entries)
	{
		switch (entry.metricEventType)
		{
			case MetricType::BeginTimedMetric:
			{
				eventStack.Push(entry);
			}break;
			case MetricType::EndTimedMetric:
			{
				const MetricEvent& beginEvent = eventStack.Peek();
				Assert(beginEvent.metricID == entry.metricID);
				eventStack.Pop();

				ProfileMetric profiledMetric = {};
				profiledMetric.frameNumber = frameNumber;
				profiledMetric.metricName = beginEvent.metricName;
				profiledMetric.lineNumber = beginEvent.lineCount;
				profiledMetric.filename = beginEvent.filename;
				profiledMetric.metricHitCount = beginEvent.hitCount;
				profiledMetric.totalMetricTimeMS = GetMillisecondsFrom(entry.cycleCount - beginEvent.cycleCount);

				mark.frameMetrics.Add(profiledMetric);
			}break;
		}
	}
}

const ProfiledFrameMark& ProfilerStatistics::GetPreviousFrame() const 
{
	u32 index = (currentFrameIndex - 1) % frameProfileCount;
	return frameProfiles[index];
}

ProfilerStatistics& GetProfilingStatistics()
{
	return stats;
}
