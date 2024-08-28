// Copyright 2020, Nathan Blane

#pragma once

// TODO - Move this file out of the Archiver project

#include "Math/Matrix4.hpp"

constexpr const u32 BoneNameLength = 64;

struct SkeletonHeader
{
	u32 boneCount = 0;
	u32 boneHierarchyOffset = 0;
	u32 boneTableOffset = 0;
	u32 bonePoseOffset = 0;
};

struct SingleBoneData
{
	tchar boneName[BoneNameLength];
	i32 parentIndex;
};

struct BonePoseData
{
	Matrix4 inversePoseMatrix;
};

