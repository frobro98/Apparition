
#include "SkeletonManager.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
// TODO - Remove this dependency
#include "Graphics/UniformBuffers.h"

#include "Archiver/SkeletonHeader.h"
#include "PCSTree/PCSNode.h"
//#include "PCSTree/PCSTree.h"

#include "Debugging/DebugOutput.hpp"

Skeleton* SkeletonManager::CreateSkeleton(u8* skeletonData, u32 hash)
{
	Skeleton* skel = FindSkeleton(hash);
	if (skel == nullptr)
	{
		//PCSTree<SkeletonBone> boneHierarchy;
		SkeletonHeader* header = reinterpret_cast<SkeletonHeader*>(skeletonData);
		u32 boneCount = header->boneCount;
		u8* boneHierarchyData = skeletonData + header->boneHierarchyOffset;
		SingleBoneData* boneData = reinterpret_cast<SingleBoneData*>(boneHierarchyData);

		SkeletonBone* bones = new SkeletonBone[boneCount];
		for (u32 i = 0; i < boneCount; ++i)
		{
			if (boneData[i].parentIndex < 0)
			{
				//bones[i].name = boneData[i].boneName;
				//boneHierarchy.Insert(&bones[i], nullptr);
			}
			else
			{
				//bones[i].name = boneData[i].boneName;
				//i32 parentIndex = boneData[i].parentIndex;
				//boneHierarchy.Insert(&bones[i], &bones[parentIndex]);
			}
			bones[i].SetIndex(i);
		}

		Assert(header->boneTableOffset > 0);
		u8* tableData = skeletonData + header->boneTableOffset;
		u32 maxElementLength = *(reinterpret_cast<u32*>(tableData));
		tableData += sizeof(u32);
		Assert(maxElementLength < BoneHierarchyTable::BoneTableElement::MaxElementLength);

		BoneHierarchyTable table;
		for (u32 i = 0; i < boneCount; ++i)
		{
			u32 numParents = *(reinterpret_cast<u32*>(tableData));
			tableData += sizeof(u32);
			//table.table[i].numParents = numParents;

			u32 j;
			for (j = 0; j < numParents; ++j)
			{
				table.table[i].boneParentHierarchy[j] = *(reinterpret_cast<u32*>(tableData));
				tableData += sizeof(i32);
			}
			for (; j < BoneHierarchyTable::BoneTableElement::MaxElementLength; ++j)
			{
				// Allows there to be an identity calculation in the shader
				table.table[i].boneParentHierarchy[j] = static_cast<u32>(boneCount);
			}
		}

		BonePoseData* poses = nullptr;
		if (header->bonePoseOffset > 0)
		{
			u8* poseData = skeletonData + header->bonePoseOffset;

			poses = new BonePoseData[boneCount];
			size_t poseCountBytes = sizeof(BonePoseData) * boneCount;
			Memcpy(poses, poseCountBytes, poseData, poseCountBytes);
		}

		skel = new Skeleton(bones, poses, table, boneCount, hash/*, MOVE(boneHierarchy)*/);

		Instance().AddSkeleton(skel);

	}

	return skel;
}

void SkeletonManager::RemoveSkeleton(Skeleton* skeleton)
{
	UNUSED(skeleton);
}

Skeleton* SkeletonManager::FindSkeleton(u32 hash)
{
	Skeleton* skel = nullptr;
	for (auto& skeleton : Instance().skeletons)
	{
		if (skeleton->GetHash() == hash)
		{
			skel = skeleton;
			break;
		}
	}

	return skel;
}

void SkeletonManager::AddSkeleton(Skeleton* skeleton)
{
	skeletons.Add(skeleton);
}

SkeletonManager& SkeletonManager::Instance()
{
	static SkeletonManager skManager;
	return skManager;
}
