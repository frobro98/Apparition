// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Archiver/FileSerializer.hpp"
#include "Archiver/FileDeserializer.hpp"

enum class Chunk
{
	VERTS_TYPE,
	WEIGHTS_TYPE,
	SKEL_TYPE,
	ANIM_TYPE,
	TEXTURE_TYPE
};

constexpr u32 ChunkNameSize = 64;

struct ChunkHeader
{
	char     chunkName[ChunkNameSize];
	u32   chunkSize;
	u32   hashNum;
	Chunk	 type;
};

forceinline void Deserialize(DeserializeBase& ser, ChunkHeader& header)
{
	ser.DeserializeData(header.chunkName, ChunkNameSize);
	Deserialize(ser, header.chunkSize);
	Deserialize(ser, header.hashNum);
	u32 e;
	Deserialize(ser, e);
	// TODO - There needs to be some sort of serialization scheme for enum types
	header.type = (Chunk)e;
}
