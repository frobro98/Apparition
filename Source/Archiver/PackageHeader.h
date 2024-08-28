// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Archiver/FileSerializer.hpp"
#include "Archiver/FileDeserializer.hpp"

constexpr u32 PackageVersionSize = 64;
constexpr u32 PackageNameSize = 64;

struct PackageHeader
{
	char	packageName[PackageNameSize];
	char	versionString[PackageVersionSize];
	u32	numChunks;
	u32	totalSize;   // size of file (without package header)
};

forceinline void Deserialize(DeserializeBase& ser, PackageHeader& header)
{
	ser.DeserializeData(header.packageName, PackageNameSize);
	ser.DeserializeData(header.versionString, PackageVersionSize);
	Deserialize(ser, header.numChunks);
	Deserialize(ser, header.totalSize);
}
