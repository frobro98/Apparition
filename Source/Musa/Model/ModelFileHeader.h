// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "Math/BoundsVolumes.hpp"

constexpr u32 ObjectNameSize = 32;
constexpr u32 TextureNameSize = 32;
constexpr u32 TextureCount = 5;

#pragma pack(push, 1)
struct ModelFileHeader
{
	// object name
	char         objName[ObjectNameSize];

	// vertex buffer
	u32 numVerts;
	u32 vertBufferOffset;

	// trilist index 
	u32 numFaces;
	u32 facesBufferOffset;

	SphereBounds boundingSphere;

	ModelFileHeader()
	{
		Memset(this, 0x0, sizeof(ModelFileHeader));
	}
};
#pragma pack(pop)
