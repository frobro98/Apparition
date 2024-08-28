// Copyright 2020, Nathan Blane

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Platform/PlatformDefinitions.h"
#include "FileSystem.hpp"
#include "Debugging/Assertion.hpp"

unsigned int FileModeToWin32Access(FileMode mode)
{
	switch (mode)
	{
	case FileMode::Read:
		return GENERIC_READ;
	case FileMode::Write:
		return GENERIC_WRITE;
	case FileMode::ReadWrite:
		return GENERIC_READ | GENERIC_WRITE;
	default:
		return 0;
	}
}

unsigned int FileLocationToWin32FP(FileLocation location)
{
	switch (location)
	{
	case FileLocation::Begin:
		return FILE_BEGIN;
	case FileLocation::Current:
		return FILE_CURRENT;
	case FileLocation::End:
		return FILE_END;
	default:
		return 0xffffffff;
	}
}

// TODO - There needs to be some way to pass in flags that dictate the file behavior. This is pretty simplistic
bool FileSystem::OpenFile( FileSystem::Handle &fh, const tchar * const fileName, FileMode mode )
{
	DWORD fileAccess = FileModeToWin32Access(mode);

	fh = CreateFile(
		fileName, 
		fileAccess, 
		FILE_SHARE_READ, 
		nullptr, 
		mode == FileMode::Read ? OPEN_EXISTING : (DWORD)OPEN_ALWAYS,
		mode == FileMode::Read ? FILE_ATTRIBUTE_READONLY : (DWORD)FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	return fh != INVALID_HANDLE_VALUE;
}

bool FileSystem::CloseFile( FileSystem::Handle fh )
{
	return CloseHandle(fh);
}

bool FileSystem::WriteFile( FileSystem::Handle fh, const void* buffer, u32 inSize )
{
	Assert(buffer);

	DWORD bytesWritten;
	bool result = ::WriteFile(fh, buffer, inSize, &bytesWritten, nullptr);
	if (result == false)
	{
		// TODO - Logging
	}

	return result;
}

bool FileSystem::ReadFile( FileSystem::Handle fh,  void* buffer, u32 size)
{
	Assert(buffer);

	DWORD bytesRead;
	bool result = ::ReadFile(fh, buffer, size, &bytesRead, nullptr);

	if (result == false)
	{
		// TODO - Logging
	}

	return result;
}

u64 FileSystem::FileSize(FileSystem::Handle fh)
{
	LARGE_INTEGER fileSize;
	GetFileSizeEx(fh, &fileSize);
	return fileSize.QuadPart;
}

bool FileSystem::SeekFile( FileSystem::Handle fh, FileLocation location, i32 offset )
{
	DWORD moveMethod = FileLocationToWin32FP(location);
	DWORD result = SetFilePointer(fh, offset, nullptr, moveMethod);
	if (result == INVALID_SET_FILE_POINTER)
	{
		// TODO - Logging
	}

	return result != INVALID_SET_FILE_POINTER;
}

u32 FileSystem::TellFile( FileSystem::Handle fh)
{
	u32 offset = SetFilePointer(fh, 0, nullptr, FILE_CURRENT);
	if (offset == INVALID_SET_FILE_POINTER)
	{
		// TODO - Logging
	}

	return offset;
}

bool FileSystem::FlushFile( FileSystem::Handle fh )
{
	bool result = FlushFileBuffers(fh);
	if (result == false)
	{
		// TODO - Logging
	}

	return result;
}

bool FileSystem::MakeDirectory(const Path& path)
{
	return ::CreateDirectory(path.GetString(), nullptr);
}

bool FileSystem::RemoveDirectory(const Path& path)
{
	return ::RemoveDirectory(path.GetString());
}

bool FileSystem::DoesDirectoryExist(const Path& path)
{
	if (!path.IsEmpty())
	{
		DWORD attrib = GetFileAttributes(path.GetString());
		return attrib != INVALID_FILE_ATTRIBUTES &&
			!!(attrib & FILE_ATTRIBUTE_DIRECTORY);
	}
	return false;
}

bool FileSystem::DoesFileExist(const Path& path)
{
	if (!path.IsEmpty())
	{
		DWORD attrib = GetFileAttributes(path.GetString());
		return attrib != INVALID_FILE_ATTRIBUTES &&
			!(attrib & FILE_ATTRIBUTE_DIRECTORY);
	}
	return false;
}

