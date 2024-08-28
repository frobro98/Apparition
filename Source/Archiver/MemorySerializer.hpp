#pragma once

#include "Containers/Array.h"
#include "Serialization/SerializeBase.hpp"

class MemorySerializer : public SerializeBase
{
public:
	MemorySerializer();

	inline uint32 DataSize() const { return serializedMemory.Size(); }

	virtual void SerializeData(void* data, uint64 dataSize) override final;

//	Array<uint8>

private:
	Array<uint8> serializedMemory;
};