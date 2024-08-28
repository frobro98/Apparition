#pragma once

#include "Time/Time.h"

struct KeyFrame;

class AnimationMixer
{
public:
	AnimationMixer(u32 skelBoneCount);
	~AnimationMixer();

	void InterpolateAnimationFrames(Time deltaTime, const KeyFrame* frameA, const KeyFrame* frameB);
	KeyFrame* GetResultingFrame() const;

private:
	KeyFrame* resultingFrame;

};