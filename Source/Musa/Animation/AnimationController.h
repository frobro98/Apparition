#pragma once

#include "Containers/DynamicArray.hpp"
#include "Time/Time.h"
#include "Clip.h"

class AnimationClip;
class AnimationTimeController;
class BlendSpace;
struct KeyFrame;

struct KeyFrameRange
{
	KeyFrameRange() = default;
	KeyFrameRange(const KeyFrame& f0, const KeyFrame& f1, float delta)
		: frame0(&f0), frame1(&f1), deltaTime(delta)
	{}
	const KeyFrame* frame0 = nullptr;
	const KeyFrame* frame1 = nullptr;
	float deltaTime = 0.f;
};

class AnimationController
{
public:
	AnimationController(Time::Duration frameRate);
	~AnimationController();

	void AddClip(AnimationClip* clip);
	void SetCurrentClip(const char* clipName);
	void SetCurrentClip(AnimationClip* clip);
	void SetCurrentClip(u32 clipIndex);

	AnimationTimeController* CreateTimeController(u32 clipIndex);
	void DestroyTimeController(AnimationTimeController* timer);
	void SetCurrentTimer(AnimationTimeController& timer);

	BlendSpace* StartBlending(u32 currentIndex, u32 newClipIndex);
	void DestroyBlendSpace(BlendSpace* blendSpace);

	void PlayAnimation();
	void PauseAnimation();

	KeyFrameRange GetFrameRange() const { return keyframeRange; }
	u32 GetClipCount() const { return clips.Size(); }
	
	KeyFrame* UpdateCurrentAnimation(Time frameTime);

private:
	void FindCurrentClipRange();

private:
	KeyFrameRange keyframeRange;
	DynamicArray<AnimationClip*> clips;
	AnimationClip* currentClip = nullptr;
	AnimationTimeController* currentTimer = nullptr;
	Time::Duration frameRate;
	u32 currentClipIndex;
	bool playing = true;
	bool pad[3] = { false, false, false };
};