// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/ConcurrentTypes.hpp"

// TODO - This container doesn't actually work. It needs to be tested and debugged a loooooootttt
template <class Elem, u32 size>
class ConcurrentQueue
{
	static_assert(size > 1, "Less than 2 elements isn't much of a queue! Add more elements");
public:
	using PushCallback = void(*)(Elem& loc, Elem&&);
	using PopCallback = void(*)(Elem&&, Elem&);

	ConcurrentQueue()
	{
		readLoc.store(0, std::memory_order_relaxed);
		readBarrier.store(0, std::memory_order_relaxed);
		writeLoc.store(0, std::memory_order_relaxed);
		writeBarrier.store(size, std::memory_order_relaxed);
	}

	void WaitPush(Elem&& item);
	void WaitPop(Elem& item);
	void WaitPushWithCallback(Elem&& item, PushCallback& cb);
	void WaitPopWithCallback(Elem& item, PopCallback& cb);

	bool TryPush(Elem&& item);
	bool TryPop(Elem& item);
	bool TryPushWithCallback(Elem&& item, PushCallback&& cb);
	bool TryPopWithCallback(Elem& item, PopCallback&& cb);

private:
	Elem data[size];
	uatom32 readLoc;
	uatom32 writeLoc;
	uatom32 readBarrier;
	uatom32 writeBarrier;
};

template<class Elem, u32 size>
inline void ConcurrentQueue<Elem, size>::WaitPush(Elem&& item)
{
	while(!TryPush(FORWARD(Elem, item))) {}
}

template<class Elem, u32 size>
inline void ConcurrentQueue<Elem, size>::WaitPop(Elem& item)
{
	while (!TryPop(item)) {}
}

template<class Elem, u32 size>
inline void ConcurrentQueue<Elem, size>::WaitPushWithCallback(Elem && item, PushCallback& cb)
{
	while (!TryPush(item, cb)) {}
}

template<class Elem, u32 size>
inline void ConcurrentQueue<Elem, size>::WaitPopWithCallback(Elem& item, PopCallback& cb)
{
	while (!TryPop(item, cb)) {}
}

template<class Elem, u32 size>
inline bool ConcurrentQueue<Elem, size>::TryPush(Elem&& item)
{
	constexpr u32 PushOffset = 0;
	const u32 pushLoc = writeLoc.load(std::memory_order_acquire);
	const u32 pushBarrier = writeBarrier.load(std::memory_order_acquire);
	if (pushLoc != pushBarrier)
	{
		if (writeLoc.compare_exchange_strong(pushLoc, pushLoc + 1, std::memory_order_release, std::memory_order_relaxed))
		{
			// Can write
			u32 index = pushLoc % size;
			data[index] = FORWARD(Elem, item);

			// Needs to spin to cover edge case of incomplete write, full write, full read
			//uint32 expected
			u32 expected = pushLoc + PushOffset;
			while (!readBarrier.compare_exchange_weak(expected, expected + 1, std::memory_order_release, std::memory_order_relaxed))
			{
				expected = pushLoc + PushOffset;
			}
			return true;
		}
	}
	return false;
}

template<class Elem, u32 size>
inline bool ConcurrentQueue<Elem, size>::TryPop(Elem& item)
{
	constexpr u32 PopOffset = size;
	u32 popBarrier = readBarrier.load(std::memory_order_acquire);
	u32 popLoc = writeBarrier.load(std::memory_order_acquire);
	if (popBarrier != popLoc)
	{
		if (readLoc.compare_exchange_strong(popLoc, popLoc+ 1, std::memory_order_release, std::memory_order_relaxed))
		{
			// Can read
			u32 index = (popLoc - 1) % size;
			item = MOVE(data[index]);

			// Needs to spin to cover edge case of incomplete write, full write, full read
			u32 expected = popLoc + PopOffset;
			while (!writeBarrier.compare_exchange_weak(expected, expected + 1, std::memory_order_release, std::memory_order_relaxed))
			{
				expected = popLoc + PopOffset;
			}
			return true;
		}
	}
	return false;
}

template <class Elem, u32 size>
inline bool ConcurrentQueue<Elem, size>::TryPushWithCallback(Elem&& item, PushCallback&& cb)
{
	constexpr u32 PushOffset = 0;
	const u32 pushLoc = writeLoc.load(std::memory_order_acquire);
	const u32 pushBarrier = writeBarrier.load(std::memory_order_acquire);
	if (pushLoc != pushBarrier)
	{
		if (writeLoc.compare_exchange_strong(pushLoc, pushLoc + 1, std::memory_order_release, std::memory_order_relaxed))
		{
			// Can write
			u32 index = pushLoc % size;
			cb(data[index], MOVE(item));

			// Needs to spin to cover edge case of incomplete write, full write, full read
			//uint32 expected
			u32 expected = pushLoc + PushOffset;
			while (!readBarrier.compare_exchange_weak(expected, expected + 1, std::memory_order_release, std::memory_order_relaxed))
			{
				expected = pushLoc + PushOffset;
			}
			return true;
		}
	}
	return false;
}

template <class Elem, u32 size>
inline bool ConcurrentQueue<Elem, size>::TryPopWithCallback(Elem& item, PopCallback&& cb)
{
	constexpr u32 PopOffset = size;
	u32 popBarrier = readBarrier.load(std::memory_order_acquire);
	u32 popLoc = writeBarrier.load(std::memory_order_acquire);
	if (popBarrier != popLoc)
	{
		if (readLoc.compare_exchange_strong(popLoc, popLoc + 1, std::memory_order_release, std::memory_order_relaxed))
		{
			// Can read
			u32 index = (popLoc - 1) % size;
			cb(MOVE(data[index]), item);

			// Needs to spin to cover edge case of incomplete write, full write, full read
			u32 expected = popLoc + PopOffset;
			while (!writeBarrier.compare_exchange_weak(expected, expected + 1, std::memory_order_release, std::memory_order_relaxed))
			{
				expected = popLoc + PopOffset;
			}
			return true;
		}
	}
	return false;
}
