
#include "AnimationTimeController.h"
#include "Time/Time.h"
#include "Math/MathFunctions.hpp"

AnimationTimeController::AnimationTimeController(Time::Duration animationFrameRate)
	: internalTime(Time(Time::ZERO)),
	animationFrame(Time(animationFrameRate))
{
}

void AnimationTimeController::Start(Time duration)
{
	animationDuration = duration;
	Reset();
}

void AnimationTimeController::Pause()
{
	playing = false;
	
}

void AnimationTimeController::Unpause()
{
	playing = true;
}

void AnimationTimeController::Reset()
{
	internalTime = Time(Time::ZERO);
	deltaTime = .5f;
}

void AnimationTimeController::UpdateTime()
{
	if (playing)
	{
		internalTime += deltaTime * animationFrame;
		if (internalTime > animationDuration)
		{
			internalTime = Time(Time::ZERO);
		}
		else if (internalTime < Time(Time::ZERO))
		{
			internalTime = animationDuration;
		}
	}
}

Time AnimationTimeController::QueryTime() const
{
	return internalTime;
}

void AnimationTimeController::Faster()
{
	deltaTime += SpeedUpDelta;
	Math::Clamp(deltaTime, -1.f, 1.f);
}

void AnimationTimeController::Slower()
{
	deltaTime += SlowDownDelta;
	Math::Clamp(deltaTime, -1.f, 1.f);
}
