// Copyright 2020, Nathan Blane

#include "Shader.hpp"
#include "ShaderResourceManager.hpp"

ShaderID Shader::FindOrLoadShaderFile(const tchar* shaderFile)
{
	ShaderID id;
	ShaderResourceManager& shaderManager = GetShaderResourceManager();
	if (!shaderManager.TryFindShaderID(shaderFile, id))
	{
		id = shaderManager.LoadShaderFile(shaderFile);
	}

	return id;
}
