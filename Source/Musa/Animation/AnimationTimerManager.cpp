#include "AnimationTimerManager.h"
#include "AnimationTimeController.h"

void AnimationTimerManager::AddTimer(AnimationTimeController* controller)
{
	Instance().timers.Add(controller);
}

void AnimationTimerManager::RemoveTimer(AnimationTimeController * controller)
{
	// TODO - add actual find functionality
	for (u32 i = 0; i < Instance().timers.Size(); ++i)
	{
		if (Instance().timers[i] == controller)
		{
			Instance().timers.Remove(i);
			break;
		}
	}
}

void AnimationTimerManager::Update(Time frameTime)
{
	for (u32 i = 0; i < Instance().timers.Size(); ++i)
	{
		Instance().timers[i]->UpdateTime();
	}
}

AnimationTimerManager& AnimationTimerManager::Instance()
{
	static AnimationTimerManager atInstance;
	return atInstance;
}

AnimationTimerManager::~AnimationTimerManager()
{
	for (u32 i = 0; i < Instance().timers.Size(); ++i)
	{
		delete Instance().timers[i];
	}
}
