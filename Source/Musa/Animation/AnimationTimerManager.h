#pragma once

#include "Containers/DynamicArray.hpp"
#include "Time/Time.h"

class AnimationTimeController;

class AnimationTimerManager
{
public:
	static void AddTimer(AnimationTimeController* controller);
	static void RemoveTimer(AnimationTimeController* controller);

	static void Update(Time frameTime);

private:
	static AnimationTimerManager& Instance();

	AnimationTimerManager() = default;
	~AnimationTimerManager();

private:
	DynamicArray<AnimationTimeController*> timers;
};