// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/MemoryBuffer.hpp"
#include "Serialization/SerializeBase.hpp"
#include "CoreAPI.hpp"

class CORE_API MemorySerializer : public SerializeBase
{
public:
	MemorySerializer(MemoryBuffer& memory);

	virtual void SerializeData(const void* data, size_t dataSize) override;

	MemoryBuffer& GetSerializedData() const;

private:
	MemoryBuffer& memoryData;
};