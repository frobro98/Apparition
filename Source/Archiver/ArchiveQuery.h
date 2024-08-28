// Copyright 2020, Nathan Blane

#pragma once

#include "ChunkHeader.h"
#include "Archiver/ArchiverDll.hpp"

ARCHIVE_API bool ProcessPackage(
	const char * const filePath,
	Chunk type,
	const char * const chunkName,
	u8* &chunkBuff,
	u32& chunkSize,
	u32& hash
);

