#pragma once

#include "Containers/DynamicArray.hpp"
//#include "PCSTree/PCSTree.h"
#include "Animation/Skeleton/SkeletonBone.h"
#include "Time/Time.h"
#include "Animation/AnimationController.h"
// TODO - remove this dependency
#include "Graphics/UniformBuffers.h"


class AnimationClip;
struct BonePoseData;
class Matrix4;

class Skeleton
{
public:
	Skeleton(SkeletonBone* boneList, BonePoseData* poseList, BoneHierarchyTable& table, u32 boneCount, u32 skeletonHash/*, PCSTree<SkeletonBone>&& hierarchy*/);
	~Skeleton();

	Skeleton(const Skeleton&) = delete;
	Skeleton& operator=(const Skeleton&) = delete;

	KeyFrameRange GetFrameRange() const { return controller->GetFrameRange(); }
	SkeletonBone* GetSkeletonBoneList() const { return bones; }
	u32 GetBoneCount() const { return boneCount; }
	DynamicArray<BonePoseData> GetPose() const { return poseMatrices; }
	const BoneHierarchyTable& GetHierarchyTable() const { return hierarchyTable; }

	u32 GetHash() const { return skeletonHash; }

	bool IsAnimationDirty() const;

	void SetWorld(const Matrix4& m);
	void AddAnimation(AnimationClip* clip);
	void SetDebug(bool debug);
	void Update(Time frameTime);

	AnimationController* GetController() const;

private:

	// TODO - These pose functions should probably go into the bone class
	void SetDebugBonePoses();
	void SetDebugBonePose(SkeletonBone* bone);

private:
	const BoneHierarchyTable hierarchyTable;
	DynamicArray<BonePoseData> poseMatrices;
	//PCSTree<SkeletonBone> boneHierarchy;
	SkeletonBone* bones;
	Matrix4* skeletonWorld;
	AnimationController* controller = nullptr;
	u32 boneCount;
	u32 skeletonHash;
	bool isDebug;

};