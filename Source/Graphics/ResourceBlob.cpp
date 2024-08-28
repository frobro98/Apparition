// Copyright 2020, Nathan Blane

#include "ResourceBlob.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "Serialization/SerializeBase.hpp"
#include "Serialization/DeserializeBase.hpp"
#include "Memory/MemoryAllocation.hpp"
#include "Memory/MemoryFunctions.hpp"
#include "Debugging/Assertion.hpp"

ResourceBlob::ResourceBlob(u8* blobData, size_t blobSize)
	: size(blobSize)
{
	Assert(blobData != nullptr);
	Assert(blobSize > 0);
	data = (u8*)Memory::Malloc(blobSize);
	Memory::Memcpy(data, blobData, blobSize);
}

ResourceBlob::ResourceBlob(const ResourceBlob& other)
	: data(other.data),
	size(other.size)
{
}

ResourceBlob::ResourceBlob(ResourceBlob&& other) noexcept
	:data(other.data),
	size(other.size)
{
	other.data = nullptr;
	other.size = 0;
}

ResourceBlob::~ResourceBlob()
{
	Memory::Free(data);
}

ResourceBlob& ResourceBlob::operator=(const ResourceBlob& other)
{
	if (this != &other)
	{
		delete data;
		data = other.data;
		size = other.size;
	}

	return *this;
}

ResourceBlob& ResourceBlob::operator=(ResourceBlob&& other) noexcept
{
	if (this != &other)
	{
		data = other.data;
		size = other.size;
		other.data = nullptr;
		other.size = 0;
	}

	return *this;
}

void ResourceBlob::MergeWith(const ResourceBlob& other)
{
	size_t newSize = size + other.size;
	u8* tmp = (u8*)Memory::Malloc(newSize);

	Memcpy(tmp, size, data, size);
	Memcpy(tmp + size, other.size, other.data, other.size);

	delete data;
	data = tmp;
	size = newSize;
}

ResourceBlob CombineBlobs(const ResourceBlob& blob0, const ResourceBlob& blob1)
{
	size_t newSize = blob0.size + blob1.size;
	u8* data = (u8*)Memory::Malloc(newSize);

	Memcpy(data, blob0.size, blob0.data, blob0.size);
	Memcpy(data + blob0.size, blob1.size, blob1.data, blob1.size);

	return ResourceBlob(data, newSize);
}

void Serialize(SerializeBase& ser, const ResourceBlob& blob)
{
	Serialize(ser, blob.GetData(), blob.GetSize());
}

void Deserialize(DeserializeBase& ser, ResourceBlob& blob)
{
	u8* data;
	size_t size;
	Deserialize(ser, data, size);
	blob = ResourceBlob(data, size);
}
