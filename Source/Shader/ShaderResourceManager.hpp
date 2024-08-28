// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/Map.h"
#include "Shader/ShaderID.hpp"
#include "Shader/ShaderAPI.hpp"

class ShaderResource;

class SHADER_API ShaderResourceManager
{
public:
	ShaderResourceManager() = default;
	~ShaderResourceManager();

	// shaderName is a path locally to the shader, not within the file system
	ShaderID LoadShaderFile(const tchar* shaderName);
	bool TryFindShaderID(const tchar* shaderName, ShaderID& id);
	ShaderResource* FindShaderResource(ShaderID id);
private:
	Map<const tchar*, ShaderID> pathToIdMap;
	Map<ShaderID, ShaderResource*> resourceMap;
};

SHADER_API ShaderResourceManager& GetShaderResourceManager();

namespace Shader
{
// TODO - This function exhibits private behavior that is public to the masses, i.e. Musa engine and games. REvisit this...
SHADER_API ShaderResource* FindAssociatedShaderResource(ShaderID id);
}