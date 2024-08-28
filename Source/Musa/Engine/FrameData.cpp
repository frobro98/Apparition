// Copyright 2020, Nathan Blane

#include "FrameData.hpp"
#include "Internal/FrameDataInternal.hpp"
#include "Utilities/MemoryUtilities.hpp"

namespace
{
static FrameTime frameData;
}

namespace Frame
{
void SetFrameStats(const FrameTime& frame)
{
	Memcpy(&frameData, sizeof(FrameTime), &frame, sizeof(FrameTime));
}

FrameTime GetFrameData()
{
	return frameData;
}

f32 GetTickTimeSeconds()
{
	return frameData.tickSeconds;
}
}