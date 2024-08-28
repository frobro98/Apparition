// Copyright 2020, Nathan Blane

#pragma once

#include "Path/Path.hpp"
#include "CoreAPI.hpp"

enum class FileMode : u32
{
	Read,
	Write,
	ReadWrite
};

enum class FileLocation : u32
{
	Begin,
	Current,
	End
};

class CORE_API FileSystem
{
public:

	typedef void* Handle;

public:
   static bool OpenFile( FileSystem::Handle &fh, const tchar * const fileName, FileMode mode );
   static bool CloseFile( FileSystem::Handle fh );
   static bool WriteFile( FileSystem::Handle fh, const void * const buffer, u32 inSize );
   static bool ReadFile( FileSystem::Handle fh, void * const _buffer, u32 _size );
   static u64 FileSize(FileSystem::Handle fh);
   static bool SeekFile( FileSystem::Handle fh, FileLocation location, i32 offset );
   static u32 TellFile(FileSystem::Handle fh);
   static bool FlushFile( FileSystem::Handle fh );

   static bool MakeDirectory(const Path& path);
   static bool RemoveDirectory(const Path& path);

   static bool DoesDirectoryExist(const Path& path);
   static bool DoesFileExist(const Path& path);
};
