// Copyright 2020, Nathan Blane

#include "CStringUtilities.hpp"
#include "Debugging/Assertion.hpp"

tchar* Strcpy(tchar* dest, size_t destSize, const tchar* src)
{
	strcpy_s(dest, destSize, src);
	return dest;
}

tchar* Strcat(tchar* dest, size_t sizeDest, const tchar* src, size_t sizeSrc) noexcept
{
	// TODO - Actually use the sizeSrc and sizeDest parameters. Should be used...
	UNUSED(sizeSrc, sizeDest);
	size_t destStrLen = Strlen(dest);
	Assert(sizeDest >= sizeSrc + 1);
	Assert(destStrLen <= (sizeDest - sizeSrc) + 1);

	tchar* destPtr = dest + destStrLen;
	while (*src != '\0')
	{
		*destPtr = *src;
		++src;
		++destPtr;
	}
	*destPtr = '\0';

	return dest;
}

tchar ToUpper(tchar character) noexcept
{
	return (tchar)::toupper(character);
}

tchar ToLower(tchar character) noexcept
{
	return (tchar)::tolower(character);
}

bool IsAlpha(tchar character) noexcept
{
	return isalpha(character);
}

bool IsAlnum(tchar character) noexcept
{
	return isalnum(character);
}


