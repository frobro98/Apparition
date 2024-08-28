// Copyright 2020, Nathan Blane

#include <stdio.h>

#include "ArchiveQuery.h"
#include "ChunkHeader.h"
#include "PackageHeader.h"

#include "File/FileSystem.hpp"

//---------------------------------------------------------------------------
// Leave this signature - do your EAT magic here
// Feel free to add files and methods to this project
//---------------------------------------------------------------------------

bool ProcessPackage(
		const char* const fileName,
		Chunk type,
		const char* const chunkName,
		u8* &chunkBuff,
		u32& chunkSize,
		u32& hash
	)
{
	chunkBuff = nullptr;
	chunkSize = 0;
	hash = 0;

	FileSystem::Handle fHandle;
	bool result = FileSystem::OpenFile(fHandle, fileName, FileMode::Read);
	if (!result)
	{
		return false;
	}

	PackageHeader packHeader = {};
	FileSystem::ReadFile(fHandle, &packHeader, sizeof(PackageHeader));

	char* packageData = new char[packHeader.totalSize];
	FileSystem::ReadFile(fHandle, packageData, packHeader.totalSize);

	FileSystem::CloseFile(fHandle);

	char* dataPtr = packageData;
	for (u32 i = 0; i < packHeader.numChunks; ++i)
	{
		ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(dataPtr);
		if (chunkHeader->type == type && strcmp(chunkHeader->chunkName, chunkName) == 0)
		{
			chunkSize = chunkHeader->chunkSize;
			chunkBuff = new unsigned char[chunkHeader->chunkSize];
			hash = chunkHeader->hashNum;
			void* chunkData = dataPtr + sizeof(ChunkHeader);
			memcpy(chunkBuff, chunkData, chunkSize);
			break;
		}

		dataPtr += chunkHeader->chunkSize + sizeof(ChunkHeader);
	}

	delete[] packageData;

	if (chunkBuff == nullptr)
	{
		return false;
	}

	return true;
}

bool ProcessHash(const char* fileName, Chunk type, const char* chunkName, u32& hash)
{
	hash = 0;
	FileSystem::Handle fHandle;
	bool result = FileSystem::OpenFile(fHandle, fileName, FileMode::Read);
	if (!result)
	{
		// TODO - Logging
		return false;
	}

	PackageHeader packHeader = {};
	FileSystem::ReadFile(fHandle, &packHeader, sizeof(PackageHeader));

	char* packageData = new char[packHeader.totalSize];
	FileSystem::ReadFile(fHandle, packageData, packHeader.totalSize);

	FileSystem::CloseFile(fHandle);

	char* dataPtr = packageData;
	for (u32 i = 0; i < packHeader.numChunks; ++i)
	{
		ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(dataPtr);
		if (chunkHeader->type == type && strcmp(chunkHeader->chunkName, chunkName) == 0)
		{
			hash = chunkHeader->hashNum;
			break;
		}

		dataPtr += chunkHeader->chunkSize + sizeof(ChunkHeader);
	}

	delete[] packageData;

	return true;
}
