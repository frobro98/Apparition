// Copyright 2020, Nathan Blane

#pragma once

#include "Path/Path.hpp"
#include "File/FileSystem.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "Serialization/SerializeBase.hpp"
#include "Archiver/ArchiverDll.hpp"

class ARCHIVE_API FileSerializer final : public SerializeBase
{
public:
	FileSerializer(const Path& filePath);
	~FileSerializer();

	virtual void SerializeData(const void* data, size_t dataSize) override final;

	inline size_t TotalSize() { return serializedData.Size(); }

private:
	void Flush();

private:
	MemoryBuffer serializedData;
	Path pathToFile;
	FileSystem::Handle handle;
	u32 bufferWriteIndex = 0;
};