#pragma once

#include "Threading/ReadWriteLock.hpp"
#include "Debugging/Assertion.hpp"

// Scoped lock for write operations on a ReadWriteLock
class ScopedWriteLock
{
public:
	ScopedWriteLock(ReadWriteLock& mut)
		: mutex(mut)
	{
		REF_CHECK(mutex);
		mutex.LockWrite();
	}

	~ScopedWriteLock()
	{
		REF_CHECK(mutex);
		mutex.UnlockWrite();
	}

private:
	ReadWriteLock& mutex;
};
