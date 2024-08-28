// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/Map.h"
#include "String/String.h"
#include "Shader/ShaderStructure.hpp"
#include "Shader/ShaderAPI.hpp"

// Describing a resource and location in the table
struct ShaderResourceInfo
{
	ShaderResourceType type;
	u16 tableIndex;
	u16 bindIndex;
	u16 size;
};

inline void Serialize(SerializeBase& ser, const ShaderResourceInfo& resInfo)
{
	Serialize(ser, (u16)resInfo.type);
	Serialize(ser, resInfo.tableIndex);
	Serialize(ser, resInfo.bindIndex);
	Serialize(ser, resInfo.size);
}

inline void Deserialize(DeserializeBase& ser, ShaderResourceInfo& resInfo)
{
	u16 type;
	Deserialize(ser, type);
	resInfo.type = (ShaderResourceType)type;
	Deserialize(ser, resInfo.tableIndex);
	Deserialize(ser, resInfo.bindIndex);
	Deserialize(ser, resInfo.size);
}

struct SHADER_API ShaderConstantNameMap
{
	void AddShaderResource(const tchar* resName, const ShaderResourceInfo& resInfo);
	ShaderResourceInfo GetResourceDescription(const String& name) const;

	friend void Serialize(SerializeBase& ser, const ShaderConstantNameMap& nameMap)
	{
		Serialize(ser, nameMap.map);
	}

	friend void Deserialize(DeserializeBase& ser, ShaderConstantNameMap& nameMap)
	{
		Deserialize(ser, nameMap.map);
	}

	Map<String, ShaderResourceInfo> map;
};

