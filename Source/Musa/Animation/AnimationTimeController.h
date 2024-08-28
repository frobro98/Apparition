#pragma once

#include "Time/Time.h"

class AnimationTimeController
{
public:
	AnimationTimeController(Time::Duration animationFrameRate);
	~AnimationTimeController() = default;

	void Start(Time animationDuration);
	void Pause();
	void Unpause();
	void Reset();

	void UpdateTime();
	Time QueryTime() const;

	// Speed controls
	void Faster();
	void Slower();

private:
	static constexpr const float SpeedUpDelta = .05f;
	static constexpr const float SlowDownDelta = -SpeedUpDelta;

	Time internalTime;
	Time animationDuration;
	Time animationFrame;
	float deltaTime = .5f;
	bool playing = true;
	bool pad[3];
};