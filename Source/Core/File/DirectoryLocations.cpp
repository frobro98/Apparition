// Copyright 2020, Nathan Blane

#include "Platform/PlatformDefinitions.h"
#include "File/DirectoryLocations.hpp"
#include "String/CStringUtilities.hpp"
#include "Utilities/Array.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

const tchar* EngineExeFullPath()
{
	// TODO - SHould use some sort of macro that determines max path size possible!
	static tchar pathResult[512] = {};

	if (pathResult[0] == 0)
	{
		HMODULE hCurrentModule;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&EngineAssetPath, &hCurrentModule);
		GetModuleFileName(hCurrentModule, pathResult, (DWORD)ArraySize(pathResult));
		size_t pathLen = Strlen(pathResult);
		if (pathLen > 0)
		{
			String temp(pathResult);
			temp.Replace("\\", "/");
			Strcpy(pathResult, 512, *temp);

			--pathLen;
			for (; pathLen > 0; pathLen--)
			{
				if (pathResult[pathLen - 1] == '/' || pathResult[pathLen - 1] == '\\')
				{
					break;
				}
			}
		}
		pathResult[pathLen] = '\0';
	}

	return pathResult;
}
