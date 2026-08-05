#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "File/FileSystem.hpp"

// General Includes to help
#include "Apparition/Backbuffer.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/CommandBufferCommands.h"
#include "Apparition/Device.h"
#include "Apparition/Image.h"
#include "Apparition/Queue.h"
#include "Apparition/Pipeline.h"

// Sandbox includes
#include "Window/Window.h"

constexpr u32 maxConcurrentFrames = 2;

constexpr const tchar* SandboxRootPath()
{
	return "../../../";
}

constexpr const tchar* SandboxAssetPath()
{
	return "../../../Assets/";
}

constexpr const tchar* SandboxModelPath()
{
	return "../../../Assets/Models/";
}

constexpr const tchar* SandboxTexturePath()
{
	return "../../../Assets/Textures/";
}

constexpr const tchar* SandboxShaderBasePath()
{
	return "../../../Assets/Shaders/";
}

inline MemoryBuffer LoadShader(const char* shaderFile)
{
	MemoryBuffer shaderCode;
	FileSystem::Handle vertHandle;
	Path fullShaderPath = Path(SandboxShaderBasePath()) / shaderFile;
	if (FileSystem::OpenFile(vertHandle, fullShaderPath.GetString(), FileMode::Read))
	{
		u64 fileSize = FileSystem::FileSize(vertHandle);
		MemoryBuffer file{ fileSize };
		if (FileSystem::ReadFile(vertHandle, file.GetData(), (u32)fileSize))
		{
			file.CopyTo(shaderCode);
		}

		FileSystem::CloseFile(vertHandle);
	}

	return shaderCode;
}

// HACK: Window exposing to every example
extern Window* window;
