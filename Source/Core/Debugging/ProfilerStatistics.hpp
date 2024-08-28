// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"
#include "Containers/StaticArray.hpp"
#include "CoreAPI.hpp"

struct ProfileMetric
{
	f64 totalMetricTimeMS;
	const tchar* metricName;
	const tchar* filename;
	u32 lineNumber;
	u32 metricHitCount;
	u32 frameNumber;
};

struct ProfiledFrameMark
{
	void Clear()
	{
		frameMetrics.Clear();
	}

	//Map<MetricGroup, DynamicArray<ProfileMetric>> groupMetrics;
	DynamicArray<ProfileMetric> frameMetrics;
};

class CORE_API ProfilerStatistics
{
public:
	// Go to the next frame of data
	void ProfileFrameIncrement();
	void CollectAllFrameMetrics();
	const ProfiledFrameMark& GetPreviousFrame() const;

private:
	// 3 seconds of data for a 60fps locked game
	static constexpr u32 frameProfileCount = 300;
	StaticArray<ProfiledFrameMark, frameProfileCount> frameProfiles = {};
	u32 currentFrameIndex = 0;
};

CORE_API ProfilerStatistics& GetProfilingStatistics();
