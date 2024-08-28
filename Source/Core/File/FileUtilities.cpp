// Copyright 2020, Nathan Blane

#include "Debugging/Assertion.hpp"
#include "FileUtilities.hpp"
#include "FileSystem.hpp"

MemoryBuffer LoadFileToMemory(const tchar* filePath)
{
	FileSystem::Handle handle;
	bool result = FileSystem::OpenFile(handle, filePath, FileMode::Read);
	Assert(result);

	u64 fileSize = FileSystem::FileSize(handle);
	MemoryBuffer fileData(fileSize);
	result = FileSystem::ReadFile(handle, fileData.GetData(), (u32)fileSize);
	Assert(result);

	result = FileSystem::CloseFile(handle);
	Assert(result);

	return fileData;
}
