// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "Utilities/MemoryUtilities.hpp"

struct ResourceArray final
{
	template <typename Res>
	ResourceArray(const DynamicArray<Res>& resCollection);
	~ResourceArray()
	{
		delete byteArrayData;
	}

	const u32 elementCount;
	const u32 elementStride;
	const u32 totalByteSize;
	const u8* byteArrayData;
};

template<typename Res>
inline ResourceArray::ResourceArray(const DynamicArray<Res>& resCollection)
	: elementCount(resCollection.Size()),
	elementStride(sizeof(Res)),
	totalByteSize(resCollection.SizeInBytes())
{
	u8* collectionBytes = new u8[totalByteSize];
	Memcpy(collectionBytes, totalByteSize, resCollection.GetData(), totalByteSize);
	byteArrayData = collectionBytes;
}
