// Copyright 2020, Nathan Blane

#include "MetricInterface.hpp"
#include "BasicTypes/Color.hpp"

namespace Musa::Internal
{
	u32 MetricGroupCounter::counter = 0;
	u32 MetricCounter::counter = 0;
}

static MetricTable metricTable;

MetricTable& GetMetricTable()
{
	return metricTable;
}

void MetricTable::AddMetric(const MetricEvent& event)
{
	entries[currentTableIndex].Add(event);
}

const MetricTable::TableEntries& MetricTable::GetCurrentTable() const
{
	// TODO: insert return statement here
	return entries[currentTableIndex];
}

void MetricTable::SwapTablesForNewFrame()
{
	currentTableIndex = currentTableIndex == 0 ? 1 : 0;
	entries[currentTableIndex].Clear();
}
