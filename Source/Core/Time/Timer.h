#pragma once

#include "BasicTypes/Uncopyable.hpp"
#include "Time/CyclePerformance.hpp"
#include "CoreAPI.hpp"

class CORE_API Timer : private Uncopyable
{
public:
	forceinline void Start()
	{
		startCycles = GetCycleCount();
	}

	forceinline f64 Mark()
	{
		markedCycles = GetCycleCount();
		return GetElapsedMilliseconds();
	}

	forceinline f64 GetElapsedMicroseconds() const
	{
		Cycles elapsedCycles = markedCycles - startCycles;
		return GetMicrosecondsFrom(elapsedCycles);
	}
	forceinline f64 GetElapsedMilliseconds() const
	{
		Cycles elapsedCycles = markedCycles - startCycles;
		return GetMillisecondsFrom(elapsedCycles);
	}
	forceinline f64 GetElapsedSeconds() const
	{
		Cycles elapsedCycles = markedCycles - startCycles;
		return GetSecondsFrom(elapsedCycles);
	}

private:
	u64 startCycles;
	u64 markedCycles;
};


