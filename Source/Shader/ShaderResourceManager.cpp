// Copyright 2020, Nathan Blane

#include "ShaderResourceManager.hpp"
#include "ShaderResource.hpp"
#include "File/DirectoryLocations.hpp"
#include "Path/Path.hpp"
#include "Archiver/FileDeserializer.hpp"

ShaderResourceManager::~ShaderResourceManager()
{

}

ShaderID ShaderResourceManager::LoadShaderFile(const tchar* shaderName)
{
	Assert(pathToIdMap.Find(shaderName) == nullptr);
	Path shaderPath = Path(EngineGeneratedShaderPath()) / shaderName;

	FileDeserializer deserializer(shaderPath);
	
	ShaderHeader header;
	Deserialize(deserializer, header);
	MemoryBuffer buffer;
	Deserialize(deserializer, buffer);

	pathToIdMap.Add(shaderName, header.id);
	resourceMap.Add(header.id, new ShaderResource(header, buffer));

	return header.id;
}

bool ShaderResourceManager::TryFindShaderID(const tchar* shaderName, ShaderID& id)
{
	ShaderID* foundID = pathToIdMap.Find(shaderName);
	if (foundID)
	{
		id = *foundID;
	}
	return foundID != nullptr;
}

ShaderResource* ShaderResourceManager::FindShaderResource(ShaderID id)
{
	ShaderResource** res = resourceMap.Find(id);
	return res != nullptr ? *res : nullptr;
}

ShaderResourceManager& GetShaderResourceManager()
{
	static ShaderResourceManager shaderManager;
	return shaderManager;
}

namespace Shader
{
ShaderResource* FindAssociatedShaderResource(ShaderID id)
{
	return GetShaderResourceManager().FindShaderResource(id);
}
}
