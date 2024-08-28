#include "SkeletonInstance.h"
#include "Archiver/SkeletonHeader.h"
#include "Skeleton.h"
#include "Animation/BlendSpace.h"
#include "Animation/AnimationController.h"
#include "Animation/AnimationTimeController.h"

SkeletonInstance::SkeletonInstance(Skeleton& skel)
	: skeleton(skel),
	controller(skel.GetController())
{
	timer = controller->CreateTimeController(0);
}

SkeletonInstance::~SkeletonInstance()
{
	controller->DestroyTimeController(timer);
	controller = nullptr;
	timer = nullptr;
}

void SkeletonInstance::SetCurrentAnimation(u32 animationIndex)
{
	controller->DestroyTimeController(timer);
	currentAnimationIndex = animationIndex;
	timer = controller->CreateTimeController(currentAnimationIndex);
}

Time SkeletonInstance::QueryDeltaTime() const
{
	return timer->QueryTime();
}

void SkeletonInstance::SetDebug(bool isDebug)
{
	skeleton.SetDebug(isDebug);
}

void SkeletonInstance::Blend()
{
	u32 totalClipCount = controller->GetClipCount();
	controller->SetCurrentTimer(*timer);
	blendSpace = controller->StartBlending(currentAnimationIndex, (currentAnimationIndex + 1) % totalClipCount);
	blendSpace->Start();
}

bool SkeletonInstance::IsBlending() const
{
	return blendSpace != nullptr;
}

KeyFrameRange SkeletonInstance::GetFrameRange()
{
	return frameRange;
}

KeyFrameRange SkeletonInstance::GetBlendRange()
{
	Assert(IsBlending());
	return blendRange;
}

float SkeletonInstance::GetBlendDelta() const
{
	return blendSpace->GetBlendDelta();
}

u32 SkeletonInstance::GetBoneCount() const
{
	return skeleton.GetBoneCount();
}

const BoneHierarchyTable& SkeletonInstance::GetHierarchyTable() const
{
	return skeleton.GetHierarchyTable();
}

DynamicArray<BonePoseData> SkeletonInstance::GetPose() const
{
	return skeleton.GetPose();
}

void SkeletonInstance::GoToNextClip()
{
	u32 totalClipCount = controller->GetClipCount();
	currentAnimationIndex = (currentAnimationIndex + 1) % totalClipCount;
	controller->DestroyTimeController(timer);
	timer = controller->CreateTimeController(currentAnimationIndex);
}

void SkeletonInstance::SetWorld(const Matrix4& world)
{
	skeleton.SetWorld(world);
}

void SkeletonInstance::Update(Time deltaTime)
{
	controller->SetCurrentTimer(*timer);
	controller->SetCurrentClip(currentAnimationIndex);
	if (!blendSpace)
	{
		skeleton.Update(deltaTime);
		frameRange = controller->GetFrameRange();
	}
	else
	{
		if (blendSpace->IsBlending())
		{
			blendSpace->Update();
			frameRange = blendSpace->GetFromRange();
			blendRange = blendSpace->GetToRange();
		}
		else
		{
			AnimationTimeController* newTimer = blendSpace->GetBlendToTimer();
			controller->DestroyTimeController(timer);
			timer = newTimer;
			currentAnimationIndex = (currentAnimationIndex + 1) % controller->GetClipCount();
			controller->DestroyBlendSpace(blendSpace);
			blendSpace = nullptr;
		}
	}
}

void SkeletonInstance::Faster()
{
	timer->Faster();
}

void SkeletonInstance::Slower()
{
	timer->Slower();
}
