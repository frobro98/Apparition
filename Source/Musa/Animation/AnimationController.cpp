
#include "AnimationController.h"
#include "AnimationMixer.h"
#include "Clip.h"
#include "AnimationTimeController.h"
#include "AnimationTimerManager.h"
#include "BlendSpace.h"
#include "String/CStringUtilities.hpp"

AnimationController::AnimationController(Time::Duration rate)
	: frameRate(rate)
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::AddClip(AnimationClip* clip)
{
	clips.Add(clip);
	currentClip = clip;
	currentClipIndex = clips.Size() - 1;
}

void AnimationController::SetCurrentClip(const char* clipName)
{
	bool found = false;
	for (u32 i = 0; i < clips.Size(); ++i)
	{
		if (Strcmp(clipName, clips[i]->GetName()) == 0)
		{
			found = true;
			currentClip = clips[i];
			currentClipIndex = i;
			break;
		}
	}

	Assert(found);
}

void AnimationController::SetCurrentClip(AnimationClip* clip)
{
	bool found = false;
	for (u32 i = 0; i < clips.Size(); ++i)
	{
		if (clip == clips[i])
		{
			found = true;
			currentClip = clips[i];
			currentClipIndex = i;
			break;
		}
	}

	Assert(found);
}

void AnimationController::SetCurrentClip(u32 clipIndex)
{
	Assert(clipIndex < clips.Size());
	currentClip = clips[clipIndex];
}

AnimationTimeController* AnimationController::CreateTimeController(u32 clipIndex)
{
	Assert(clipIndex < clips.Size());

	AnimationTimeController* timer = new AnimationTimeController(frameRate);
	timer->Start(clips[clipIndex]->GetTotalAnimationTime());
	AnimationTimerManager::AddTimer(timer);
	
	return timer;
}

void AnimationController::DestroyTimeController(AnimationTimeController* timerToKill)
{
	AnimationTimerManager::RemoveTimer(timerToKill);
	delete timerToKill;
}

void AnimationController::SetCurrentTimer(AnimationTimeController& newTimer)
{
	currentTimer = &newTimer;
}

BlendSpace* AnimationController::StartBlending(u32 currentIndex, u32 newClipIndex)
{
	BlendSpace* blend = new BlendSpace();
	blend->SetBlendTargets(*clips[currentIndex], *currentTimer, *clips[newClipIndex]);

	return blend;
}

void AnimationController::DestroyBlendSpace(BlendSpace* blendSpace)
{
	delete blendSpace;
}

void AnimationController::PlayAnimation()
{
	currentTimer->Unpause();
}

void AnimationController::PauseAnimation()
{
	currentTimer->Pause();
}

KeyFrame* AnimationController::UpdateCurrentAnimation(Time /*frameTime*/)
{
	Assert(currentTimer);
	Time deltaTime = currentTimer->QueryTime();
	currentClip->UpdateKeyframeRange(deltaTime);
	float deltaS = (deltaTime - currentClip->GetKeyframe0().frameTime) / (currentClip->GetKeyframe1().frameTime - currentClip->GetKeyframe0().frameTime);
	keyframeRange = KeyFrameRange(currentClip->GetKeyframe0(), currentClip->GetKeyframe1(), deltaS);

	//currentClip->InterpolateKeyframes(deltaTime);

	return nullptr;
}
