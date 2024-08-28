// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"
#include "CoreAPI.hpp"

// TODO - Path doesn't necessarily need to contain a String. That's wasteful and the OS also
//		  has a max path size limit. Once I know this limit, I can optimize this to use a fixed
//		  array of characters
class CORE_API Path
{
public:
	Path() = default;
	Path(const Path&) = default;
	Path(Path&&) = default;
	Path& operator=(const Path&) = default;
	Path& operator=(Path&&) = default;

	Path(const tchar* strPath);
	Path(String&& strPath);

	bool DoesFileExist() const;
	bool DoesDirectoryExist() const;
	String GetFileExtension() const;
	String GetFileName() const;
	String GetFileNameWithoutExtension() const;
	void Normalize();
	Path GetNormalized() const;
	void MakeAbsolute();
	Path GetAbsolute() const;
	Path GetDirectoryPath() const;
	DynamicArray<String> Split() const;
	const tchar* GetString() const;
	forceinline bool IsEmpty() const
	{
		return path.IsEmpty();
	}

	// Operators
	Path& operator/=(const Path& otherPath);
	Path& operator/=(const String& str);
	Path& operator/=(const tchar* str);
	
private:
	void Append(const tchar* path);
	bool IsSlash(tchar c) const;
	bool HasEndingSlash(const String& pathStr) const;

	friend CORE_API Path operator/(const Path& lhPath, const Path& rhPath);
	friend CORE_API Path operator/(const Path& path, const String& str);
	friend CORE_API Path operator/(const Path& path, const String& str);
	friend CORE_API Path operator/(const String& str, const Path& path);
	friend CORE_API Path operator/(const String& str, const Path& path);
	friend CORE_API Path operator/(const Path& path, const tchar* str);
	friend CORE_API Path operator/(const Path& path, const tchar* str);
	friend CORE_API Path operator/(const tchar* str, const Path& path);
	friend CORE_API Path operator/(const tchar* str, const Path& path);

private:
	String path;
};