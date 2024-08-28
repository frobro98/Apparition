// Copyright 2020, Nathan Blane

#pragma once

#include <limits>
#include <intrin.h>

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/StaticArray.hpp"
#include "Containers/DynamicArray.hpp"
#include "Time/CyclePerformance.hpp"
#include "Utilities/MacroHelpers.hpp"
#include "CoreAPI.hpp"

enum class MetricType : u32
{
	BeginTimedMetric,
	EndTimedMetric
};

struct MetricEvent
{
	// Represents the cycles it took the block to complete
	u64 cycleCount;
	const tchar* metricName;
	const tchar* filename;
	u32 lineCount;
	u32 hitCount;
	u32 metricID;
	MetricType metricEventType;

	friend bool operator==(const MetricEvent& e0, const MetricEvent& e1) { return e0.metricID == e1.metricID; }
};

constexpr u64 MetricTableEntryCount = std::numeric_limits<u16>::max();

class CORE_API MetricTable
{
public:
	using TableEntries = DynamicArray<MetricEvent>;
public:
	
	void AddMetric(const MetricEvent& event);
	const TableEntries& GetCurrentTable() const;
	void SwapTablesForNewFrame();

private:
	static constexpr u32 metricTableCount = 2;
	StaticArray<TableEntries, metricTableCount> entries;
	u32 currentTableIndex = 0;
};

CORE_API MetricTable& GetMetricTable();

//////////////////////////////////////////////////////////////////////////

// TODO - Think of better names for the metric recording interface

using MetricID = u32;

namespace Musa::Internal
{
class CORE_API MetricGroupCounter
{
public:
	static u32 GetNewMetricID() { return counter++; }

private:
	static u32 counter;
};

class CORE_API MetricCounter
{
public:
	static u32 GetNewMetricID() { return counter++; }

private:
	static u32 counter;
};
}

#define METRIC_NAME(Name) metric_##Name
#define METRIC_GROUP_NAME(Name) MetricGroup_##Name

#define DECLARE_METRIC_GROUP_STRUCT(Name)					\
	struct MetricGroup_##Name										\
	{																\
		static inline u32 GetGroupID() { static const u32 id = Musa::Internal::MetricGroupCounter::GetNewMetricID(); return id; }			\
		static inline const char* GetGroupName() { return #Name; }	\
	}

#define DECLARE_METRIC_STRUCT(Name, GroupName)		\
	struct Metric_##Name									\
	{														\
		using MetricGroup = METRIC_GROUP_NAME(GroupName);	\
		inline const char* GetName() { return #Name; }		\
		inline u32 GetID() { static u32 id = Musa::Internal::MetricGroupCounter::GetNewMetricID(); return id; }	\
	}

#define DECLARE_METRIC_GROUP(Name)	\
	DECLARE_METRIC_GROUP_STRUCT(Name);

#define METRIC_STAT(Name, GroupName)	\
	DECLARE_METRIC_STRUCT(Name, GroupName);	\
	static Metric_##Name metric_##Name;

forceinline void BeginTimedBlock(const char* name, u32 id, const char* fileName, u32 lineNum)
{
	MetricTable& table = GetMetricTable();
	MetricEvent metric = {};
	metric.metricID = id;
	metric.cycleCount = GetCycleCount();
	metric.metricName = name;
	metric.filename = fileName;
	metric.lineCount = lineNum;
	metric.metricEventType = MetricType::BeginTimedMetric;
	++metric.hitCount;
	table.AddMetric(metric);
}

forceinline void EndTimedBlock(u32 id)
{
	MetricTable& table = GetMetricTable();					
	MetricEvent metric = {};
	metric.metricID = id;
	metric.cycleCount = GetCycleCount();
	metric.metricName = nullptr;
	metric.filename = nullptr;
	metric.lineCount = 0;
	metric.metricEventType = MetricType::EndTimedMetric;
	table.AddMetric(metric);
}


#define BEGIN_TIMED_BLOCK(Name) \
	BeginTimedBlock(#Name, METRIC_NAME(Name).GetID(), __FILE__, __LINE__)

#define END_TIMED_BLOCK(Name) \
	EndTimedBlock(METRIC_NAME(Name).GetID());

class CORE_API ScopedTimeMetric
{
public:
	ScopedTimeMetric(const char* name, u32 id_, const tchar* filename, u32 lineNumber)
		: id(id_)
	{
		BeginTimedBlock(name, id, filename, lineNumber);
	}

	~ScopedTimeMetric()
	{
		EndTimedBlock(id);
	}

private:
	u32 id;
};

#define SCOPED_TIMED_BLOCK(Name)	\
	ScopedTimeMetric timedBlock_##Name(#Name, metric_##Name.GetID(), __FILE__, __LINE__);

