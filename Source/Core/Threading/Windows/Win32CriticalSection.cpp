
#include "Threading/CriticalSection.hpp"

WALL_WRN_PUSH
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
WALL_WRN_POP

CriticalSection::CriticalSection()
{
	Win32::InitializeCriticalSectionAndSpinCount(&critSection, 0x400);
}

CriticalSection::~CriticalSection()
{
	Win32::DeleteCriticalSection(&critSection);
}

void CriticalSection::Lock()
{
	Win32::EnterCriticalSection(&critSection);
}

bool CriticalSection::TryLock()
{
	return Win32::TryEnterCriticalSection(&critSection);
}

void CriticalSection::Unlock()
{
	Win32::LeaveCriticalSection(&critSection);
}
