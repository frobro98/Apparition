#include "BlendSpace.h"
#include "Clip.h"
#include "AnimationTimeController.h"
#include "AnimationTimerManager.h"


void BlendSpace::SetBlendTargets(AnimationClip& blendingFrom, AnimationTimeController& fromTimer, AnimationClip& blendingTo)
{
	blendFrom = &blendingFrom;
	blendTo = &blendingTo;
	blendFromTimer = &fromTimer;
	blendingToTimer = new AnimationTimeController(blendingTo.GetFrameRate());
	blendingToTimer->Start(blendTo->GetTotalAnimationTime());
	AnimationTimerManager::AddTimer(blendingToTimer);
}

AnimationClip& BlendSpace::GetBlendFromClip()
{
	return *blendFrom;
}

AnimationTimeController* BlendSpace::GetBlendToTimer()
{
	return blendingToTimer;
}

void BlendSpace::Start()
{
	currentlyBlending = true;
}

void BlendSpace::Update()
{
	if (currentlyBlending)
	{
		Time fromDelta = blendFromTimer->QueryTime();
		Time toDelta = blendingToTimer->QueryTime();
		blendFrom->UpdateKeyframeRange(fromDelta);
		blendTo->UpdateKeyframeRange(toDelta);

		float fromDeltaS = (fromDelta - blendFrom->GetKeyframe0().frameTime) / (blendFrom->GetKeyframe1().frameTime - blendFrom->GetKeyframe0().frameTime);
		blendFromRange = KeyFrameRange(blendFrom->GetKeyframe0(), blendFrom->GetKeyframe1(), fromDeltaS);
		float toDeltaS = (toDelta - blendTo->GetKeyframe0().frameTime) / (blendTo->GetKeyframe1().frameTime - blendTo->GetKeyframe0().frameTime);
		blendToRange = KeyFrameRange(blendTo->GetKeyframe0(), blendTo->GetKeyframe1(), toDeltaS);

		if (toDelta <= Time(Time::ONE_SECOND))
		{
			blendingDelta = (toDelta - Time(Time::ZERO)) / (Time(Time::ONE_SECOND) - Time(Time::ZERO));
		}
		else
		{
			currentlyBlending = false;
		}
	}
}

bool BlendSpace::IsBlending()
{
	return currentlyBlending;
}

KeyFrameRange BlendSpace::GetFromRange() const
{
	return blendFromRange;
}

KeyFrameRange BlendSpace::GetToRange() const
{
	return blendToRange;
}
