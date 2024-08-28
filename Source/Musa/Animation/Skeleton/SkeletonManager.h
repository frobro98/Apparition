#pragma once

#include "String/String.h"
#include "Containers/DynamicArray.hpp"

class Skeleton;

class SkeletonManager
{
public:
	static Skeleton* CreateSkeleton(u8* skeletonData, u32 hash);
	static void RemoveSkeleton(Skeleton* skeleton);
	static Skeleton* FindSkeleton(u32 hash);

private:
	void AddSkeleton(Skeleton* skeleton);

	SkeletonManager() = default;
	~SkeletonManager() = default;

	static SkeletonManager& Instance();

private:
	DynamicArray<Skeleton*> skeletons;

};