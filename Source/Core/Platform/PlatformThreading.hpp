#pragma once

// For atomic_thread_fence...
#include <atomic>
#include "BasicTypes/Intrinsics.hpp"

class NativeThread;
class ISyncEvent;

namespace PlatformThreading
{
NativeThread* CreateThread();
ISyncEvent* CreateSyncEvent(bool isManualReset);
u32 GetCurrentThreadID();
u32 GetNumberOfHWThreads();

// Atomic operations
i8  InterlockedExchange(volatile i8* dest, i8 value);
i16 InterlockedExchange(volatile i16* dest, i16 value);
i32 InterlockedExchange(volatile i32* dest, i32 value);
i64 InterlockedExchange(volatile i64* dest, i64 value);
void* InterlockedExchangePointer(void* volatile* dest, void* value);

i8  InterlockedCompareExchange(volatile i8* dest, i8 value, i8 expected);
i16 InterlockedCompareExchange(volatile i16* dest, i16 value, i16 expected);
i32 InterlockedCompareExchange(volatile i32* dest, i32 value, i32 expected);
i64 InterlockedCompareExchange(volatile i64* dest, i64 value, i64 expected);
void* InterlockedCompareExchangePointer(void* volatile* dest, void* value, void* expected);

// i8 InterlockedIncrement(volatile i8* addend);
// i8 InterlockedDecrement(volatile i8* addend);

i16 InterlockedIncrement(volatile i16* addend);
i16 InterlockedDecrement(volatile i16* addend);

i32 InterlockedIncrement(volatile i32* addend);
i32 InterlockedDecrement(volatile i32* addend);

i64 InterlockedIncrement(volatile i64* addend);
i64 InterlockedDecrement(volatile i64* addend);

i8 InterlockedExchangeAdd(volatile i8* dest, i8 value);
i32 InterlockedExchangeAdd(volatile i16* dest, i16 value);
i32 InterlockedExchangeAdd(volatile i32* dest, i32 value);
i64 InterlockedExchangeAdd(volatile i64* dest, i64 value);

i8 InterlockedAnd(volatile i8* dest, i8 value);
i8 InterlockedOr(volatile i8* dest, i8 value);
i8 InterlockedXor(volatile i8* dest, i8 value);

i16 InterlockedAnd(volatile i16* dest, i16 value);
i16 InterlockedOr(volatile i16* dest, i16 value);
i16 InterlockedXor(volatile i16* dest, i16 value);

i32 InterlockedAnd(volatile i32* dest, i32 value);
i32 InterlockedOr(volatile i32* dest, i32 value);
i32 InterlockedXor(volatile i32* dest, i32 value);

i64 InterlockedAnd(volatile i64* dest, i64 value);
i64 InterlockedOr(volatile i64* dest, i64 value);
i64 InterlockedXor(volatile i64* dest, i64 value);

// _ReadWriteBarrier() is how acquire/release semantics should work
// Memory Fences
forceinline void AcquireFence()
{
	std::atomic_thread_fence(std::memory_order_acquire);
}

forceinline void ReleaseFence()
{
	std::atomic_thread_fence(std::memory_order_release);
}

forceinline void MemoryFence()
{
	std::atomic_thread_fence(std::memory_order_seq_cst);
}

}
