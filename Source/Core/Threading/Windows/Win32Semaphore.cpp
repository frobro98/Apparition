
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Threading/Semaphore.hpp"
#include "Debugging/Assertion.hpp"

static bool WaitOnSemaphore(HANDLE semaphore, u32 msWait)
{
	const u32 waitResult = ::WaitForSingleObject(semaphore, msWait);
	switch (waitResult)
	{
		case WAIT_OBJECT_0: return true;
		case WAIT_TIMEOUT: return false;
		case WAIT_ABANDONED: Assert(false); return false;
		case WAIT_FAILED: Assert(false); return false;
		default:
			Assert(false);
			return false;
	}
}

Semaphore::Semaphore(i32 startingCount)
	: semaphore((void*)::CreateSemaphore(nullptr, startingCount, I32Max, nullptr))
{
	Assert(startingCount >= 0);
}

Semaphore::Semaphore(i32 startingCount, i32 maxCount)
	: semaphore((void*)::CreateSemaphore(nullptr, startingCount, maxCount, nullptr))
{
	Assert(startingCount >= 0);
	Assert(maxCount >= 0);
}

Semaphore::~Semaphore()
{
	CloseHandle((HANDLE)semaphore);
}

void Semaphore::Wait(u32 msWait)
{
	WaitOnSemaphore((HANDLE)semaphore, msWait);
}

bool Semaphore::TryWait()
{
	return WaitOnSemaphore((HANDLE)semaphore, 0);
}

u32 Semaphore::Signal()
{
	u32 prevSignalCount;
	::ReleaseSemaphore((HANDLE)semaphore, 1, (LPLONG)&prevSignalCount);
	return prevSignalCount;
}