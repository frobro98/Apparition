
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Win32NativeThread.hpp"
#include "Threading/IThreadExecution.hpp"
#include "Threading/ISyncEvent.hpp"
#include "Platform/PlatformThreading.hpp"

namespace
{
// TODO - Apparently, this isn't 
const u32 MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
struct THREADNAME_INFO
{
	u32 dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	u32 dwThreadID; // Thread ID (-1=caller thread).
	u32 dwFlags; // Reserved for future use, must be zero.
};
#pragma pack(pop)

static void SetThreadName(u32 threadID, const char* threadName) {
	THREADNAME_INFO info = {};
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = threadID;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
#pragma warning(pop)
}
}

static i32 GetWin32ThreadPriority(ThreadPriority priority)
{
	switch (priority)
	{
		case ThreadPriority::High: return THREAD_PRIORITY_HIGHEST;
		case ThreadPriority::Normal: return THREAD_PRIORITY_NORMAL;
		case ThreadPriority::Low: return THREAD_PRIORITY_LOWEST;

		default:
		{
			Assert(false);
			return THREAD_PRIORITY_NORMAL;
		}
	}
}

Win32NativeThread::~Win32NativeThread()
{
	if (threadHandle != nullptr)
	{
		WaitStop();
	}
}

void Win32NativeThread::StartWithBody(const tchar* name, ThreadPriority priority, IThreadExecution& execution)
{
	REF_CHECK(execution);
	executionBody = &execution;
	threadName = name;

	threadHandle = ::CreateThread(nullptr, 0, ThreadProc, this, 
		STACK_SIZE_PARAM_IS_A_RESERVATION | CREATE_SUSPENDED, (LPDWORD)&threadId);
	Assert(threadHandle);

	syncEvent = PlatformThreading::CreateSyncEvent(true);
	Assert(syncEvent);

	// Thread starts suspended
	ResumeThread();
	// TODO - Might want to keep these somewhere to pool them? Somewhere on StackOverflow, it says that these
	// events are limited. Don't know the limit but supposedly are limited...
	syncEvent->Wait();

	SetThreadName(threadId, *threadName);
	SetPriority(priority);

	delete syncEvent;
}

void Win32NativeThread::SetPriority(ThreadPriority priority)
{
	i32 win32Prioity = GetWin32ThreadPriority(priority);
	threadPriority = priority;
	::SetThreadPriority(threadHandle, win32Prioity);
}

void Win32NativeThread::ResumeThread()
{
	::ResumeThread(threadHandle);
}

void Win32NativeThread::SuspendThread()
{
	::SuspendThread(threadHandle);
}

void Win32NativeThread::WaitStop()
{
	if (executionBody)
	{
		executionBody->RequestStop();

		WaitJoin();

		::CloseHandle(threadHandle);
		threadHandle = nullptr;
	}
}

void Win32NativeThread::WaitJoin()
{
	WaitForSingleObject(threadHandle, INFINITE);
}

DWORD Win32NativeThread::ThreadProc(LPVOID parameter)
{
	return reinterpret_cast<Win32NativeThread*>(parameter)->StartRun();
}

u32 Win32NativeThread::StartRun()
{
	u32 exitCode = 1;
	Assert(executionBody);
	Assert(syncEvent);

	executionBody->ThreadInit();
	syncEvent->Set();

	executionBody->ThreadBody();

	executionBody->ThreadExit();

	return exitCode;
}
