// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/MemoryBuffer.hpp"
#include "Serialization/DeserializeBase.hpp"
#include "CoreAPI.hpp"

class CORE_API MemoryDeserializer : public DeserializeBase
{
public:
	MemoryDeserializer(const MemoryBuffer& memory);

	virtual void DeserializeData(void* data, size_t dataSize) override;

	const MemoryBuffer& GetSerializedData() const;

private:
	const MemoryBuffer& memoryData;
	u64 bufferReadLoc = 0;
};