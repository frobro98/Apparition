// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

// C-string functions
CORE_API tchar* Strcpy(tchar* dest, size_t destSize, const tchar* src);
CORE_API tchar* Strcat(tchar* dest, size_t sizeDest, const tchar* src, size_t sizeSrc) noexcept;

template<size_t sizeDest>
forceinline tchar* Strcat(tchar(&dest)[sizeDest], const tchar* src, u32 sizeSrc) noexcept
{
	return Strcat(dest, sizeDest, src, sizeSrc);
}

// TODO - will probably need to revisit for unicode
constexpr size_t Strlen(const tchar* str) noexcept
{
	size_t len = 0;
	if (str != nullptr)
	{
		while (*str++)
		{
			++len;
		}
	}
	return len;
}

constexpr i32 Strcmp(const tchar* str1, const tchar* str2) noexcept
{
	if (str1 == nullptr)
	{
		return -1;
	}
	if (str2 == nullptr)
	{
		return 1;
	}
	size_t len1 = Strlen(str1);
	size_t len2 = Strlen(str2);
	size_t maxLen = len1 > len2 ? len1 : len2;
	for (; maxLen; --maxLen, ++str1, ++str2)
	{
		if (*str1 < *str2)
		{
			return -1;
		}
		if (*str2 < *str1)
		{
			return 1;
		}
	}
	return 0;
}

constexpr i32 Strncmp(const tchar* str1, const tchar* str2, size_t count) noexcept
{
	for (; count; --count, ++str1, ++str2)
	{
		if (*str1 < *str2)
		{
			return -1;
		}
		if (*str2 < *str1)
		{
			return 1;
		}
	}
	return 0;
}

constexpr const tchar* Strchr(const tchar* str, tchar character) noexcept
{
	const tchar* res = nullptr;
	while (*str)
	{
		if (*str == character)
		{
			res = str;
			break;
		}
		++str;
	}
	return res;
}

constexpr const tchar* Strrchr(const tchar* str, tchar character) noexcept
{
	const size_t strLen = Strlen(str);
	const tchar* res = nullptr;
	const tchar* start = str + strLen - 1;
	while (start >= str)
	{
		if (*start == character)
		{
			res = start;
			break;
		}
		--start;
	}
	return res;
}

constexpr const tchar* Strstr(const tchar* str, const tchar* findStr) noexcept
{
	const tchar* res = nullptr;
	while (*str != 0)
	{
		if (*str == *findStr)
		{
			const tchar* foundPtr = str;
			const tchar* findPtr = findStr + 1;
			while (*findPtr)
			{
				++str;
				if (*findPtr != *str)
				{
					break;
				}
				++findPtr;
			}

			if (*findPtr == 0)
			{
				res = foundPtr;
				break;
			}
		}
		++str;
	}
	return res;
}

constexpr tchar* Strstr(tchar* str, const tchar* findStr) noexcept
{
	tchar* res = nullptr;
	while (*str != 0)
	{
		if (*str == *findStr)
		{
			res = str;
			const tchar* findPtr = findStr + 1;
			while (*findPtr)
			{
				++str;
				if (*findPtr != *str)
				{
					break;
				}
				++findPtr;
			}

			if (*findPtr == 0)
			{
				break;
			}
		}
		++str;
	}
	return res;
}

// TODO - Move this into a header that is essentially only consumed by String
constexpr i32 FindFirstIn(const tchar* str, size_t strLen, const tchar* findStr, size_t findStrLen)
{
// 	Assert(str);
// 	Assert(findStr);
	if (findStrLen == 0 || findStrLen > strLen)
	{
		return -1;
	}

	for (const tchar* iter = str;; ++iter)
	{
		iter = Strstr(str, findStr);
		if (iter == nullptr)
		{
			return -1;
		}
		size_t iterLen = strLen - static_cast<u32>(iter - str);
		size_t count = iterLen < findStrLen ? iterLen : findStrLen;
		if (Strncmp(iter, findStr, count) == 0)
		{
			return static_cast<i32>(iter - str);
		}
	}
}

// TODO - Move this into a header that is essentially only consumed by String
constexpr i32 FindLastIn(const tchar* str, size_t strLen, const tchar* findStr, size_t findStrLen)
{
// 	Assert(str);
// 	Assert(findStr);
	if (findStrLen == 0 || findStrLen > strLen)
	{
		return -1;
	}

	const tchar* const endPos = str + strLen;
	for (const tchar* iter = endPos - findStrLen; iter != (str - 1); --iter)
	{
		u32 i = 0;
		for (; i < findStrLen; ++i)
		{
			if (iter[i] != findStr[i])
			{
				break;
			}
		}

		if (findStr[i] == '\0')
		{
			return static_cast<i32>(iter - str);
		}
	}

	return -1;
}

// TODO - Move this into a header that is essentially only consumed by String
constexpr bool StartsWith(const tchar* str, size_t strLen, const tchar* startStr, size_t startStrLen)
{
	if (startStrLen > strLen)
	{
		return false;
	}

	while (*startStr != '\0')
	{
		if (*startStr != *str)
		{
			return false;
		}
		++str;
		++startStr;
	}
	return true;
}

// TODO - Move this into a header that is essentially only consumed by String
constexpr bool EndsWith(const tchar* str, size_t strLen, const tchar* endStr, size_t endStrLen)
{
	if (endStrLen > strLen)
	{
		return false;
	}

	// account for null terminator
	const tchar* strData = str + strLen - 1;
	const tchar* endStrData = endStr + endStrLen - 1;
	i32 chCount = static_cast<i32>(endStrLen);

	while (chCount > 0)
	{
		if (*strData != *endStrData)
		{
			return false;
		}

		--endStrData;
		--strData;
		--chCount;
	}
	return true;
}

CORE_API tchar ToUpper(tchar character) noexcept;
CORE_API tchar ToLower(tchar character) noexcept;
CORE_API bool IsAlpha(tchar character) noexcept;
CORE_API bool IsAlnum(tchar character) noexcept;

