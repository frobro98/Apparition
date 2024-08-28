
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Animation/Clip.h"
#include "Animation/AnimationController.h"
#include "Math/MathConstants.hpp"
#include "Archiver/SkeletonHeader.h"

Skeleton::Skeleton(SkeletonBone* boneList, BonePoseData* poseList, BoneHierarchyTable& table, u32 count, u32 hash/*, PCSTree<SkeletonBone>&& hierarchy*/)
	: hierarchyTable(table),
	poseMatrices(poseList, count),
	//boneHierarchy(MOVE(hierarchy)),
	bones(boneList),
	skeletonWorld(new Matrix4(IDENTITY)),
	boneCount(count),
	skeletonHash(hash),
	isDebug(true)
{
	Assert(poseList != nullptr);
	delete[] poseList;
}

Skeleton::~Skeleton()
{
	delete[] bones;
}

void Skeleton::SetWorld(const Matrix4& m)
{
	*skeletonWorld = m;
}

void Skeleton::AddAnimation(AnimationClip* clip)
{
	Assert(clip->GetSkeletonHash() == skeletonHash);
	if (controller == nullptr)
	{
		controller = new AnimationController(clip->GetFrameRate());
	}

	controller->AddClip(clip);
}

void Skeleton::Update(Time frameTime)
{
	controller->UpdateCurrentAnimation(frameTime);
}

AnimationController * Skeleton::GetController() const
{
	return controller;
}

void Skeleton::SetDebug(bool /*debug*/)
{
// 	PCSTree<SkeletonBone>::ForwardIterator iter(boneHierarchy.GetRoot());
// 	while (!iter.IsDone())
// 	{
// 		SkeletonBone* bone = *iter;
// 		Assert(bone != nullptr);
// 		bone->SetDebug(debug);
// 
// 		++iter;
// 	}

}

void Skeleton::SetDebugBonePoses()
{
// 	PCSTree<SkeletonBone>::ForwardIterator iter(boneHierarchy.GetRoot());
// 	while (!iter.IsDone())
// 	{
// 		SkeletonBone* bone = *iter;
// 		Assert(bone != nullptr);
// 		
// 		SetDebugBonePose(bone);
// 
// 		++iter;
// 	}
}

void Skeleton::SetDebugBonePose(SkeletonBone* bone)
{
	Assert(bone != nullptr);

	const SkeletonBone* parentBone = nullptr;
// 	if (bone->parent != nullptr)
// 	{
// 		parentBone = bone->parent;
// 		Assert(parentBone);
// 	}

	if (parentBone != nullptr)
	{
		Vector4 start(0, 0, 0);

		Vector4 initialParentPoint = start * parentBone->GetWorld();
		Vector4 initialChildPoint = start * bone->GetWorld();

		// Get the direction to the parent bone
		Vector4 direction = initialParentPoint - initialChildPoint;

		float mag = direction.Magnitude();

		Vector4 up(0, 1, 0);
		up = up * parentBone->GetWorld();

		const float boneWidth = 8.f;

		Matrix4 scale(SCALE, boneWidth, mag, boneWidth);
		Quat rot(ROT_ORIENT, direction.GetNormalized(), up);
		Quat rotX(ROT_X, Math::PiOver2);
		Matrix4 trans(TRANS, initialChildPoint);

		Matrix4 orientation = scale * rotX* rot * trans;

		bone->SetOrientation(orientation);
	}
}
