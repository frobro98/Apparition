// Copyright 2020, Nathan Blane

#include "MemoryBuffer.hpp"
#include "Debugging/Assertion.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "Memory/MemoryAllocation.hpp"
#include "Serialization/SerializeBase.hpp"
#include "Serialization/DeserializeBase.hpp"

MemoryBuffer::MemoryBuffer(size_t size)
	: memSize(size)
{
	AllocMem();
}

MemoryBuffer::~MemoryBuffer()
{
	DeallocMem();
}

MemoryBuffer::MemoryBuffer(const MemoryBuffer& other)
{
	ReallocMem(other.memSize);
	memSize = other.memSize;
	Copy(other.memData, other.memSize);
}

MemoryBuffer& MemoryBuffer::operator=(const MemoryBuffer& other)
{
	if (this != &other)
	{
		memSize = other.memSize;
		ReallocMem(memSize);
		Copy(other.memData, other.memSize);
	}

	return *this;
}

// MemoryBuffer::MemoryBuffer(MemoryBuffer&& other) noexcept
// 	: memData(other.memData),
// 	memSize(other.memSize)
// {
// 	other.memData = nullptr;
// 	other.memSize = 0;
// }
// 
// MemoryBuffer& MemoryBuffer::operator=(MemoryBuffer&& other) noexcept
// {
// 	if (this != &other)
// 	{
// 		memData = other.memData;
// 		memSize = other.memSize;
// 
// 		other.memData = nullptr;
// 		other.memSize = 0;
// 	}
// 
// 	return *this;
// }

void MemoryBuffer::Add(const void* data, size_t dataSize)
{
	ReallocMem(memSize + dataSize);
	Memcpy(memData + memSize, dataSize, data, dataSize);
	memSize += dataSize;
}

u8* MemoryBuffer::Offset(size_t offset) const
{
	Assert(offset < memSize);
	return memData + offset;
}

void MemoryBuffer::IncreaseSize(size_t sizeToIncreaseBy)
{
	ReallocMem(memSize + sizeToIncreaseBy);
	memSize += sizeToIncreaseBy;
}

u8* MemoryBuffer::GetData() const
{
	return memData;
}

size_t MemoryBuffer::Size() const
{
	return memSize;
}

void MemoryBuffer::Copy(u8* data, size_t size)
{
	Memcpy(memData, data, size);
}

void MemoryBuffer::AllocMem()
{
	memData = (u8*)Memory::Malloc(memSize);
}

void MemoryBuffer::ReallocMem(size_t newSize)
{
	memData = (u8*)Memory::Realloc(memData, newSize);
}

void MemoryBuffer::DeallocMem()
{
	Memory::Free(memData);
	memData = nullptr;
}

void Serialize(SerializeBase& ser, const MemoryBuffer& buffer)
{
	Serialize(ser, buffer.memData, buffer.memSize);
}

void Deserialize(DeserializeBase& ser, MemoryBuffer& buffer)
{
	Deserialize(ser, buffer.memData, buffer.memSize);
}

