// Copyright 2020, Nathan Blane

#include "ShaderConstantNameMap.hpp"

void ShaderConstantNameMap::AddShaderResource(const tchar* resName, const ShaderResourceInfo& resInfo)
{
	map.Add(resName, resInfo);
}

ShaderResourceInfo ShaderConstantNameMap::GetResourceDescription(const String& name) const
{
	const ShaderResourceInfo* resInfo = map.Find(name);
	Assertf(resInfo != nullptr, "Resource name, {}, doesn't exist within this shader map!", *name);
	return *resInfo;
}
