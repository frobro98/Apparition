// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Serialization/DeserializeBase.hpp"
#include "Serialization/SerializeBase.hpp"
#include "Shader/ShaderTables.hpp"
#include "Shader/ShaderID.hpp"

struct ShaderHeader
{
	ShaderID id;
	ShaderStage::Type stage;
	// TODO - This doesn't contain the inputs and outputs of the shader. Put this data into the shader header
	ShaderConstantTable resourceTable;
	ShaderConstantNameMap resourceNames;
};

forceinline void Serialize(SerializeBase& ser, const ShaderHeader& shaderHeader)
{
	Serialize(ser, shaderHeader.id.shaderNameHash);
	Serialize(ser, shaderHeader.id.bytecodeHash);
	Serialize(ser, (u16)shaderHeader.stage);
	Serialize(ser, shaderHeader.resourceTable);
	Serialize(ser, shaderHeader.resourceNames);
}

forceinline void Deserialize(DeserializeBase& ser, ShaderHeader& shaderHeader)
{
	Deserialize(ser, shaderHeader.id.shaderNameHash);
	Deserialize(ser, shaderHeader.id.bytecodeHash);
	u16 stage;
	Deserialize(ser, stage);
	shaderHeader.stage = (ShaderStage::Type)stage;
	Deserialize(ser, shaderHeader.resourceTable);
	Deserialize(ser, shaderHeader.resourceNames);
}
