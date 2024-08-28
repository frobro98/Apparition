// Copyright 2020, Nathan Blane

#include "MemorySerializer.hpp"

MemorySerializer::MemorySerializer(MemoryBuffer& memory)
	: memoryData(memory)
{
}

void MemorySerializer::SerializeData(const void* data, size_t dataSize)
{
	memoryData.Add(data, dataSize);
}

MemoryBuffer& MemorySerializer::GetSerializedData() const
{
	return memoryData;
}
