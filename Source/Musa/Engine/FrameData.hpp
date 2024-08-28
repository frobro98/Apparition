// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct FrameTime
{
	// tick for this frame (not including physics at this point...)
	const f32 tickSeconds = 0;
	// Maybe what global time the frame started at...???
};

namespace Frame
{
FrameTime GetFrameTime();
f32 GetTickTimeSeconds();
}