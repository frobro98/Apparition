#pragma once

#include "Threading/CriticalSection.hpp"
#include "Debugging/Assertion.hpp"
#include "BasicTypes/Uncopyable.hpp"

class ScopedLock : private Uncopyable
{
public:
	ScopedLock(CriticalSection& criticalSection)
		: cs(criticalSection)
	{
		REF_CHECK(criticalSection);
		cs.Lock();
	}
	~ScopedLock()
	{
		cs.Unlock();
	}

private:
	CriticalSection& cs;
};
