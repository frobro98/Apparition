// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

// NOTE - This is implemented in a similar way to how std::deque would be implemented...
// If it's performance necessary, you can't go with the DynamicArray alone

constexpr u32 QueueNodeSize = 512;

// Contains the math that actually figures out how many elements
template <typename T>
static constexpr forceinline u32 GetCountPerBlock()
{
	return sizeof(T) <= QueueNodeSize ?
		QueueNodeSize / sizeof(T) : 1;
}

template <typename T>
static constexpr forceinline u32 GetBlockIndex(u32 off)
{
	// TODO - There might be an issue with just a divide here. I'm not sure there is but I might be missing something
	return off / GetCountPerBlock<T>();
}

// TODO - Support allocators

template <typename T>
class CORE_TEMPLATE Queue
{
public:
	Queue() = default;
	~Queue();

	template <class PushType>
	void Push(PushType&& elem);
	NODISCARD T& Peek();
	T Pop();

	bool IsEmpty() const;
	u32 Size() const;

private:

	forceinline void ExpandBlocks()
	{
		u32 newBlockCount = blockCount > 0 ? blockCount * 2 : 1;
		queueBlocks = (T**)Memory::Realloc(queueBlocks, sizeof(T**) * newBlockCount);
		for (u32 i = blockCount; i < newBlockCount; ++i)
		{
			queueBlocks[i] = (T*)Memory::Malloc(GetCountPerBlock<T>() * sizeof(T));
		}
		blockCount = newBlockCount;
	}

	forceinline T* GetQueueElement(u32 off)
	{
		u32 blockIndex = GetBlockIndex<T>(off);
		u32 elemIndex = off % GetCountPerBlock<T>();
		return &queueBlocks[blockIndex][elemIndex];
	}

private:
	T** queueBlocks = nullptr;
	u32 offset = 0;
	u32 size = 0;
	u32 blockCount = 0;
};

template<typename T>
forceinline Queue<T>::~Queue()
{
	while (!IsEmpty())
	{
		Pop();
	}

	for (u32 i = 0; i < blockCount; ++i)
	{
		Memory::Free(queueBlocks[i]);
	}
	Memory::Free(queueBlocks);
	
	blockCount = 0;
	queueBlocks = nullptr;
}

template<typename T>
template<class PushType>
forceinline void Queue<T>::Push(PushType&& elem)
{
	const u32 index = offset + size;
	const u32 pushBlock = GetBlockIndex<T>(index);
	if (index % GetCountPerBlock<T>() == 0 && pushBlock >= blockCount)
	{
		ExpandBlocks();
	}

	new(GetQueueElement(index)) T(FORWARD(PushType, elem));
	++size;
}

template<typename T>
forceinline T& Queue<T>::Peek()
{
	return *GetQueueElement(offset);
}

template<typename T>
forceinline T Queue<T>::Pop()
{
	Assert(size > 0);
	T elem = *GetQueueElement(offset);
	GetQueueElement(offset)->~T();
	--size;
	if (size > 0)
	{
		++offset;
	}
	else
	{
		offset = 0;
	}

	return elem;
}

template<typename T>
forceinline bool Queue<T>::IsEmpty() const
{
	return size == 0;
}

template<typename T>
forceinline u32 Queue<T>::Size() const
{
	return size;
}

