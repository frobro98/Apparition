// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

f32 NormalizeStickValue(i16 stickVal)
{
	const f32 denomVal = stickVal >= 0 ? 32767.f : 32768.f;
	return stickVal / denomVal;
}

f32 NormalizeTriggerValue(u8 triggerValue)
{
	return triggerValue / 255.f;
}

