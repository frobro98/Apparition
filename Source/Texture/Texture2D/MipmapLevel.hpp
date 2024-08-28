// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"
#include "Graphics/ResourceBlob.hpp"

struct MipmapLevel
{
	ResourceBlob mipData;
	u32 width = 0;
	u32 height = 0;
};

forceinline void Serialize(SerializeBase& ser, const MipmapLevel& level)
{
	Serialize(ser, level.mipData);
	Serialize(ser, level.width);
	Serialize(ser, level.height);
}

forceinline void Deserialize(DeserializeBase& ser, MipmapLevel& level)
{
	Deserialize(ser, level.mipData);
	Deserialize(ser, level.width);
	Deserialize(ser, level.height);
}