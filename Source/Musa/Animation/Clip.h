#pragma once


#include "Time/Time.h"
#include "KeyFrame.h"

class AnimationMixer;

class AnimationClip
{
public:
	AnimationClip(const tchar* animationName, KeyFrame* frames, u32 frameCount, Time animationTime, u32 skelHash, Time::Duration frameRate);
	~AnimationClip();

	u32 GetSkeletonHash() const;
	Time GetTotalAnimationTime() const;
	Time::Duration GetFrameRate() const;
	const char* GetName() const;

	const KeyFrame& GetKeyframe0() { return *keyframeA; }
	const KeyFrame& GetKeyframe1() { return *keyframeB; }

	void UpdateKeyframeRange(Time currentAnimTime);
	KeyFrame* InterpolateKeyframes(Time deltaTime);

private:
	static constexpr const size_t ClipNameLength = 64;
	char clipName[ClipNameLength];

	AnimationMixer* mixer;

	const KeyFrame* keyframeA;
	const KeyFrame* keyframeB;
	u32 currentFrameBIndex;

	Time totalAnimationTime;
	const KeyFrame* animationFrames;
	u32 skeletonHash;
	u32 frameCount;

	Time::Duration frameRate;
};