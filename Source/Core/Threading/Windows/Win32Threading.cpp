
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Platform/PlatformThreading.hpp"
#include "Threading/Windows/Win32NativeThread.hpp"
#include "Threading/Windows/Win32SyncEvent.hpp"

namespace PlatformThreading
{
NativeThread* CreateThread()
{
	return new Win32NativeThread;
}

ISyncEvent* CreateSyncEvent(bool isManualReset)
{
	ISyncEvent* event = new Win32SyncEvent;
	event->Create(isManualReset);
	if (!event->IsValid())
	{
		delete event;
		event = nullptr;
	}
	return event;
}

u32 GetCurrentThreadID()
{
	return ::GetCurrentThreadId();
}

u32 GetNumberOfHWThreads()
{
	// TODO - This call is probably something we can cache...
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

i8  InterlockedExchange(volatile i8* dest, i8 value)
{
	return ::_InterlockedExchange8((volatile char*)dest, value);
}

i16 InterlockedExchange(volatile i16* dest, i16 value)
{
	return ::_InterlockedExchange16((volatile short*)dest, value);
}

i32 InterlockedExchange(volatile i32* dest, i32 value)
{
	return ::_InterlockedExchange((volatile long*)dest, value);
}

i64 InterlockedExchange(volatile i64* dest, i64 value)
{
	return ::_InterlockedExchange64((volatile long long*)dest, value);
}

void* InterlockedExchangePointer(void* volatile* dest, void* value)
{
	return ::_InterlockedExchangePointer(dest, value);
}

i8  InterlockedCompareExchange(volatile i8* dest, i8 value, i8 expected)
{
	return ::_InterlockedCompareExchange8((volatile char*)dest, value, expected);
}

i16 InterlockedCompareExchange(volatile i16* dest, i16 value, i16 expected)
{
	return ::_InterlockedCompareExchange16((volatile short*)dest, value, expected);
}

i32 InterlockedCompareExchange(volatile i32* dest, i32 value, i32 expected)
{
	return ::_InterlockedCompareExchange((volatile long*)dest, value, expected);
}

i64 InterlockedCompareExchange(volatile i64* dest, i64 value, i64 expected)
{
	return ::_InterlockedCompareExchange64((volatile long long*)dest, value, expected);
}

void* InterlockedCompareExchangePointer(void* volatile* dest, void* value, void* expected)
{
	return ::_InterlockedCompareExchangePointer(dest, value, expected);
}

i16 InterlockedIncrement(volatile i16* addend)
{
	return ::_InterlockedIncrement16((volatile short*)addend);
}

i16 InterlockedDecrement(volatile i16* addend)
{
	return ::_InterlockedDecrement16((volatile short*)addend);
}

i32 InterlockedIncrement(volatile i32* addend)
{
	return ::_InterlockedIncrement((volatile long*)addend);
}

i32 InterlockedDecrement(volatile i32* addend)
{
	return ::_InterlockedDecrement((volatile long*)addend);
}

i64 InterlockedIncrement(volatile i64* addend)
{
	return ::_InterlockedIncrement64((volatile long long*)addend);
}

i64 InterlockedDecrement(volatile i64* addend)
{
	return ::_InterlockedDecrement64((volatile long long*)addend);
}

i8 InterlockedExchangeAdd(volatile i8 * dest, i8 value)
{
	return ::_InterlockedExchange8((volatile char*)dest, value);
}

i32 InterlockedExchangeAdd(volatile i16 * dest, i16 value)
{
	return ::_InterlockedExchange16((volatile short*)dest, value);
}

i32 InterlockedExchangeAdd(volatile i32* dest, i32 value)
{
	return ::_InterlockedExchangeAdd((volatile long*)dest, value);
}

i64 InterlockedExchangeAdd(volatile i64* dest, i64 value)
{
	return ::_InterlockedExchangeAdd64((volatile long long*)dest, value);
}

i8 InterlockedAnd(volatile i8* dest, i8 value)
{
	return ::_InterlockedAnd8((volatile char*)dest, value);
}

i8 InterlockedOr(volatile i8* dest, i8 value)
{
	return ::_InterlockedOr8((volatile char*)dest, value);
}

i8 InterlockedXor(volatile i8* dest, i8 value)
{
	return ::_InterlockedXor8((volatile char*)dest, value);
}

i16 InterlockedAnd(volatile i16* dest, i16 value)
{
	return ::_InterlockedAnd16((volatile short*)dest, value);
}

i16 InterlockedOr(volatile i16* dest, i16 value)
{
	return ::_InterlockedOr16((volatile short*)dest, value);
}

i16 InterlockedXor(volatile i16* dest, i16 value)
{
	return ::_InterlockedXor16((volatile short*)dest, value);
}

i32 InterlockedAnd(volatile i32* dest, i32 value)
{
	return ::_InterlockedAnd((volatile long*)dest, value);
}

i32 InterlockedOr(volatile i32* dest, i32 value)
{
	return ::_InterlockedOr((volatile long*)dest, value);
}

i32 InterlockedXor(volatile i32* dest, i32 value)
{
	return ::_InterlockedXor((volatile long*)dest, value);
}

i64 InterlockedAnd(volatile i64* dest, i64 value)
{
	return ::_InterlockedAnd64((volatile long long*)dest, value);

}

i64 InterlockedOr(volatile i64* dest, i64 value)
{
	return ::_InterlockedOr64((volatile long long*)dest, value);
}

i64 InterlockedXor(volatile i64* dest, i64 value)
{
	return ::_InterlockedXor64((volatile long long*)dest, value);
}
}