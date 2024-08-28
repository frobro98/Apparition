#pragma once

#include "Containers/DynamicArray.hpp"
#include "Time/Time.h"
// TODO - make this dependency just the KeyframeRange...
#include "Animation/AnimationController.h"

struct BonePoseData;
struct BoneHierarchyTable;
class Matrix4;
class Skeleton;
class AnimationController;
class AnimationTimeController;

class SkeletonInstance
{
public:
	SkeletonInstance(Skeleton& skel);
	~SkeletonInstance();

	SkeletonInstance(const SkeletonInstance&) = delete;
	SkeletonInstance& operator=(const SkeletonInstance&) = delete;

	void SetCurrentAnimation(u32 animationIndex);
	Time QueryDeltaTime() const;
	void SetDebug(bool isDebug);

	void Blend();
	bool IsBlending() const;

	KeyFrameRange GetFrameRange();
	KeyFrameRange GetBlendRange();
	float GetBlendDelta() const;
	u32 GetBoneCount() const;
	const BoneHierarchyTable& GetHierarchyTable() const;
	DynamicArray<BonePoseData> GetPose() const;
	AnimationController* GetController() { return controller; }

	void SetWorld(const Matrix4& world);
	void Update(Time deltaTime);

	void Faster();
	void Slower();
	void GoToNextClip();

private:
	KeyFrameRange frameRange;
	KeyFrameRange blendRange;
	Skeleton& skeleton;
	class BlendSpace* blendSpace = nullptr;
	AnimationController* controller;
	AnimationTimeController* timer;
	u32 currentAnimationIndex = 0;
};
