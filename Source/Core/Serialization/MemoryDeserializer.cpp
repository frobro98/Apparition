// Copyright 2020, Nathan Blane

#include "Debugging/Assertion.hpp"
#include "MemoryDeserializer.hpp"
#include "Utilities/MemoryUtilities.hpp"

MemoryDeserializer::MemoryDeserializer(const MemoryBuffer& memory)
	: memoryData(memory)
{
}

void MemoryDeserializer::DeserializeData(void* data, size_t dataSize)
{
	Assert(dataSize <= memoryData.Size() - bufferReadLoc);
	Memcpy(data, dataSize, memoryData.Offset(bufferReadLoc), dataSize);
	bufferReadLoc += dataSize;
}

const MemoryBuffer& MemoryDeserializer::GetSerializedData() const
{
	return memoryData;
}
