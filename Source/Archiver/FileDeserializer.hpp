// Copyright 2020, Nathan Blane

#pragma once

#include "Path/Path.hpp"
#include "File/FileSystem.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "Serialization/DeserializeBase.hpp"
#include "Archiver/ArchiverDll.hpp"

class ARCHIVE_API FileDeserializer final : public DeserializeBase
{
public:
	FileDeserializer(const Path& filePath);
	~FileDeserializer();

	virtual void DeserializeData(void* data, size_t dataSize) override final;

	inline size_t TotalSize() const { return fileData.Size(); }

private:
	void CacheFile();

private:
	MemoryBuffer fileData;
	Path pathToFile;
	FileSystem::Handle handle;
	u32 bufferReadLoc = 0;
};