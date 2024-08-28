// Copyright 2020, Nathan Blane

#pragma once

#include "SkeletonHeader.h"
#include "Animation/KeyFrame.h"
#include "Time/Time.h"

// TODO - Move this file out of the Archiver project

constexpr u32 AnimationNameLength = 64;

struct AnimationHeader
{
	AnimationHeader()
	{
		memset(animationName, 0, AnimationNameLength);
	}

	tchar animationName[AnimationNameLength];
	u32 referenceSkeleton;
	u32 keyFrameCount;
	u32 bonesPerFrame;
	u32 totalAnimationTime;
	Time::Duration frameRate;
};

forceinline void Deserialize(DeserializeBase& ser, AnimationHeader& header)
{
	ser.DeserializeData(header.animationName, AnimationNameLength);
	Deserialize(ser, header.referenceSkeleton);
	Deserialize(ser, header.keyFrameCount);
	Deserialize(ser, header.bonesPerFrame);
	Deserialize(ser, header.totalAnimationTime);
	u32 duration;
	Deserialize(ser, duration);
	header.frameRate = (Time::Duration)duration;
}