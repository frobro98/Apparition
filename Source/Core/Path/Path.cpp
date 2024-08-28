// Copyright 2020, Nathan Blane

#include "Path.hpp"
#include "Platform/PlatformDefinitions.h"
#include "String/CStringUtilities.hpp"
#include "File/DirectoryLocations.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace
{
constexpr const tchar* BackSlash = "\\";
constexpr const tchar* ForwardSlash = "/";
}

Path::Path(const tchar* strPath)
	: path(strPath)
{
	Assert(strPath && *strPath);
}

Path::Path(String&& strPath)
	: path(strPath)
{
	Assert(!strPath.IsEmpty());
}

bool Path::DoesFileExist() const
{
	DWORD attrib = GetFileAttributes(*path);
	return attrib != INVALID_FILE_ATTRIBUTES &&
		!(attrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool Path::DoesDirectoryExist() const
{
	DWORD attrib = GetFileAttributes(*path);
	return attrib != INVALID_FILE_ATTRIBUTES &&
		!!(attrib & FILE_ATTRIBUTE_DIRECTORY);
}

String Path::GetFileExtension() const
{
	// TODO - Implement my version of string_view so that I can get at the extension without allocating
	i32 index = path.FindLast(".");
	if (index != -1)
	{
		return String((*path) + index + 1, path.Length()+1 - index);
	}

	return String("");
}

String Path::GetFileName() const
{
	const tchar* endStr = *path + (path.Length());
	const tchar* curStr = endStr;
	while (!IsSlash(*(curStr - 1)))
	{
		--curStr;
	}
	u32 filenameSize = static_cast<u32>(endStr - curStr);
	return String(curStr, filenameSize);
}

String Path::GetFileNameWithoutExtension() const
{
	const tchar* endStr = *path + path.FindLast(".");
	const tchar* curStr = endStr;
	while (!IsSlash(*(curStr - 1)))
	{
		--curStr;
	}
	u32 filenameSize = static_cast<u32>(endStr - curStr);
	return String(curStr, filenameSize);
}

void Path::Normalize()
{
	path.Replace(BackSlash, ForwardSlash);
}

Path Path::GetNormalized() const
{
	Path p(*this);
	p.Normalize();
	return p;
}

void Path::MakeAbsolute()
{
	constexpr const tchar* parentDir = "/..";
	constexpr u32 parentStrLen = 3;
	if (path[0] != '/' && !IsAlpha(path[0]))
	{
		String tempPath(path);
		path = EngineExeFullPath();
		*this /= tempPath;
	}

	i32 index = path.FindFirst(parentDir);
	while(index != -1)
	{
		u32 prevSlashIndex = static_cast<u32>(index - 1);
		while (!IsSlash(path[prevSlashIndex]))
		{
			--prevSlashIndex;
		}
		u32 relativePathToRemove = index - prevSlashIndex + parentStrLen;
		path.Remove(prevSlashIndex, relativePathToRemove);

		index = path.FindFirst(parentDir);
	}
}

Path Path::GetAbsolute() const
{
	Path p(*this);
	p.MakeAbsolute();
	return p;
}

Path Path::GetDirectoryPath() const
{
	if (GetFileExtension().Length() > 0)
	{
		const tchar* endStr = *path + (path.Length());
		const tchar* curStr = endStr;
		u32 endIndex = path.Length();
		while (!IsSlash(*(curStr - 1)))
		{
			--curStr;
			--endIndex;
		}
		return Path(path.SubStr(0, endIndex));

	}
	else
	{
		return *this;
	}
}

DynamicArray<String> Path::Split() const
{
	return path.Split(ForwardSlash);
}

const tchar* Path::GetString() const
{
	return *path;
}

void Path::Append(const tchar* str)
{
	if (!HasEndingSlash(path))
	{
		path += "/";
	}
	if (IsSlash(str[0]))
	{
		path += &str[1];
	}
	else
	{
		path += str;
	}
}

bool Path::IsSlash(tchar c) const
{
	return (c == '/' || c == '\\');
}

bool Path::HasEndingSlash(const String& pathStr) const
{
	return IsSlash(pathStr[pathStr.Length() - 1]);
}

Path& Path::operator/=(const Path& otherPath)
{
	Append(*otherPath.path);
	return *this;
}

Path& Path::operator/=(const String& str)
{
	Append(*str);
	return *this;
}

Path& Path::operator/=(const tchar* str)
{
	Append(str);
	return *this;
}

Path operator/(const Path& lhPath, const Path& rhPath)
{
	Path path(lhPath);
	path /= rhPath;
	return path;
}

Path operator/(const Path& path, const String& str)
{
	Path p(path);
	p /= str;
	return p;
}

Path operator/(const String& str, const Path& path)
{
	Path p(*str);
	p /= path;
	return p;
}

Path operator/(const Path& path, const tchar* str)
{
	Path p(path);
	p /= str;
	return p;
}

Path operator/(const tchar* str, const Path& path)
{
	Path p(str);
	p /= path;
	return p;
}
