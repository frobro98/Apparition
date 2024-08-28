// Copyright 2020, Nathan Blane

#include "FileDeserializer.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "Logging/CoreLogChannels.hpp"
#include "Logging/LogFunctions.hpp"

FileDeserializer::FileDeserializer(const Path& filePath)
	: pathToFile(filePath)
{
	bool result = FileSystem::OpenFile(handle, pathToFile.GetString(), FileMode::Read);
	if (!result)
	{
		// TODO - Logging
		MUSA_ERR(DeserializationLog, "Failed to open file {}", *pathToFile.GetFileName());
	}
	else
	{
		CacheFile();
	}
}

FileDeserializer::~FileDeserializer()
{
	bool result = FileSystem::CloseFile(handle);
	if (!result)
	{
		// TODO - GetLastError
		MUSA_ERR(DeserializationLog, "Failed to close file {}", *pathToFile.GetFileName());
	}
}

void FileDeserializer::DeserializeData(void* data, size_t dataSize)
{
	// TODO - Doesn't really work for things over 4Gb. Brainstorm ways to fix this
	Assert(dataSize <= fileData.Size() - bufferReadLoc);
	Memcpy(data, dataSize, fileData.Offset(bufferReadLoc), dataSize);
	bufferReadLoc += (u32)dataSize;
}

void FileDeserializer::CacheFile()
{
	u64 fileSize = FileSystem::FileSize(handle);
	fileData.IncreaseSize(fileSize);

	bool result = FileSystem::ReadFile(handle, fileData.GetData(), (u32)fileSize);
	if (!result)
	{
		// TODO - GetLastError
		MUSA_ERR(DeserializationLog, "Failed to read from file {}", *pathToFile.GetFileName());
	}
}
