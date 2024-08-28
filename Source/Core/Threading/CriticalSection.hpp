#pragma once

// TODO - Remove this somehow
#include "Platform/PlatformDefinitions.h"
#include "BasicTypes/Uncopyable.hpp"

// TODO - SRWLock is more optimal than CRITICAL_SECTION. Change this to use that
class CriticalSection final : private Uncopyable
{
public:
	CriticalSection();
	~CriticalSection();

	void Lock();
	bool TryLock();
	void Unlock();

private:
	Win32::CRITICAL_SECTION critSection;
};
