#pragma once

#include "AnimationController.h"

class AnimationClip;
class AnimationTimeController;

class BlendSpace
{
public:
	BlendSpace() = default;

	void SetBlendTargets(AnimationClip& blendingFrom, AnimationTimeController& fromTimer, AnimationClip& blendingTo);
	AnimationClip& GetBlendFromClip();
	AnimationTimeController* GetBlendToTimer();
	void Start();
	void Update();
	bool IsBlending();

	KeyFrameRange GetFromRange() const;
	KeyFrameRange GetToRange() const;
	float GetBlendDelta() const { return blendingDelta; }

private:
	AnimationClip* blendFrom = nullptr;
	AnimationClip* blendTo = nullptr;
	AnimationTimeController* blendFromTimer = nullptr;
	AnimationTimeController* blendingToTimer = nullptr;
	AnimationTimeController* blendTimer = nullptr;
	KeyFrameRange blendFromRange;
	KeyFrameRange blendToRange;
	float blendingDelta;
	bool currentlyBlending = false;
	bool pad[3];
};
