#pragma once

#include "BasicTypes/Intrinsics.hpp"
class Skeleton;
class SkeletonInstance;

class AnimationGameObject// : public GameObject
{
public:
	AnimationGameObject() = default;
	~AnimationGameObject() = default;

	virtual void SetupInputs() /*override*/;

	void SetSkeleton(Skeleton& skeleton);
	void SetCurrentClip(u32 clipIndex);
	virtual void Update(float tick) /*override*/;

	void SetDebug(bool isDebug);

private:
	void TogglePause();
	void SpeedUpAnimation();
	void SlowDownAnimation();
	void SwapCurrentAnimation();
	void Blend();

private:
	SkeletonInstance* skeleton = nullptr;
	bool playing = true;
	bool pad[3] = { false, false, false };
};