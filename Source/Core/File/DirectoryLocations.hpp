// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"
#include "CoreAPI.hpp"

// TODO - figure out namespacing convention for these
// TODO - Add actual Path and Directory creation classes
//namespace Internal
//{
CORE_API const tchar* EngineExeFullPath();

inline const tchar* EngineRootPath()
{
	return "../../../";
}

inline const tchar* EngineAssetPath()
{
	return "../../../Assets/";
}

inline const tchar* EngineFontPath()
{
	return "../../../Assets/Fonts/";
}

inline const tchar* EngineLogPath()
{
	return "../../../Logs/";
}

inline const tchar* EngineShaderSrcPath()
{
	return "../../../Assets/Shaders/";
}

inline const tchar* EngineShaderProvidedPath()
{
	return "../../../Assets/Shaders/Provided/";
}

inline const tchar* EngineGeneratedShaderPath()
{
	return "../../../Assets/Shaders/Generated/";
}

inline const tchar* EngineSourcePath()
{
	return "../../../Source/";
}

inline const tchar* EngineTexturePath()
{
	return "../../../Assets/Textures/";
}

//}