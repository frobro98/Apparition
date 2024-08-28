// Copyright 2020, Nathan Blane

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "EngineTick.h"

float EngineTick::CountsPerSec = 0.f;

EngineTick::EngineTick()
{
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	// Only divides once
	CountsPerSec = 1.f / Freq.QuadPart;
}


EngineTick::~EngineTick()
{
}

void EngineTick::Start()
{
	LARGE_INTEGER Start;
	QueryPerformanceCounter(&Start);
	startTime = Start.QuadPart;
}

void EngineTick::Lap()
{
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);
	deltaTime = tick.QuadPart - startTime;
}

f64 EngineTick::GetSeconds() const
{
	f64 timeElapsed = static_cast<f64>(deltaTime);
	timeElapsed *= CountsPerSec;
	return timeElapsed;
}

f64 EngineTick::GetMilliseconds() const
{
	f64 timeElapsed = static_cast<f64>(deltaTime);
	timeElapsed *= CountsPerSec * 1000;
	return timeElapsed;
}

f64 EngineTick::GetMicroseconds() const
{
	f64 timeElapsed = static_cast<f64>(deltaTime);
	timeElapsed *= CountsPerSec * 1000000;
	return timeElapsed;
}
