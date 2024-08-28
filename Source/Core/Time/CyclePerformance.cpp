// Copyright 2020, Nathan Blane

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CyclePerformance.hpp"
#include "Platform/Windows/WindowsDefinitions.h"

static f64 InvCyclesPerSec = 0.0;

Cycles GetCycleCount()
{
	LARGE_INTEGER cycles;
	QueryPerformanceCounter(&cycles);
	return (Cycles)cycles.QuadPart;
}

f64 GetMicrosecondsFrom(Cycles cycles)
{
	// TODO - There should be some sort of initialization for timings
	if (InvCyclesPerSec == 0)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		InvCyclesPerSec = 1. / freq.QuadPart;
	}

	cycles *= 1000000;
	f64 microsec = cycles * InvCyclesPerSec;
	return microsec;
}

f64 GetMillisecondsFrom(Cycles cycles)
{
	return GetMicrosecondsFrom(cycles) / 1000.;
}

f64 GetSecondsFrom(Cycles cycles)
{
	return GetMicrosecondsFrom(cycles) / 1000000.;
}

CORE_API f64 GetMicroseconds()
{
	// TODO - There should be some sort of initialization for timings
	if (InvCyclesPerSec == 0)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		InvCyclesPerSec = 1. / freq.QuadPart;
	}

	LARGE_INTEGER cycles;
	QueryPerformanceCounter(&cycles);
	Cycles cycleCount = (Cycles)cycles.QuadPart;

	cycleCount *= 1000000;
	f64 microsec = cycleCount * InvCyclesPerSec;
	return microsec;
}

CORE_API f64 GetMilliseconds()
{
	return GetMicroseconds() / 1000.;
}

CORE_API f64 GetSeconds()
{
	return GetMicroseconds() / 1000000.;
}
