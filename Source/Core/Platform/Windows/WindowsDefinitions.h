// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

#pragma pack(push, 16)

#ifndef STRICT
#define STRICT
#endif 

#define WINAPI __stdcall

#define MIN_WIN32_API extern "C" _declspec(dllimport)

// Global Win32 defs
struct HINSTANCE__;
struct HWND__;
struct _RTL_SRWLOCK;
struct _RTL_CRITICAL_SECTION;
union _LARGE_INTEGER;

namespace Win32
{
using HANDLE = void*;
using HINSTANCE = HINSTANCE__*;

using BOOL = i32;
typedef unsigned long DWORD;
using LPDWORD = DWORD*;
using LPVOID = void*;
using LONG_PTR = i64;

typedef _RTL_CRITICAL_SECTION*  LPCRITICAL_SECTION;
using RTL_SRWLOCK = _RTL_SRWLOCK;
using PRTL_SRWLOCK = _RTL_SRWLOCK*;

struct CRITICAL_SECTION { void* void0[1]; long long0[2]; void* void1[3]; };

struct SRWLOCK
{
	void* p;
};

using LPLARGE_INTEGER = _LARGE_INTEGER*;

MIN_WIN32_API BOOL WINAPI InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);
MIN_WIN32_API void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
MIN_WIN32_API void WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
MIN_WIN32_API BOOL WINAPI TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
MIN_WIN32_API void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

forceinline BOOL WINAPI InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION* lpCriticalSection, DWORD dwSpinCount)
{
	return InitializeCriticalSectionAndSpinCount((LPCRITICAL_SECTION)lpCriticalSection, dwSpinCount);
}

forceinline void WINAPI DeleteCriticalSection(CRITICAL_SECTION* lpCriticalSection)
{
	DeleteCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
}

forceinline void WINAPI EnterCriticalSection(CRITICAL_SECTION* lpCriticalSection)
{
	EnterCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
}

forceinline BOOL WINAPI TryEnterCriticalSection(CRITICAL_SECTION* lpCriticalSection)
{
	return TryEnterCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
}

forceinline void WINAPI LeaveCriticalSection(CRITICAL_SECTION* lpCriticalSection)
{
	LeaveCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
}
}



#pragma pack(pop)