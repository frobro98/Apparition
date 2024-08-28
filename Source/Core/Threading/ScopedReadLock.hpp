#pragma once

#include "Threading/ReadWriteLock.hpp"
#include "Debugging/Assertion.hpp"

// Scoped lock for read operations on a ReadWriteLock
class ScopedReadLock
{
public:
	ScopedReadLock(ReadWriteLock& mut)
		: mutex(mut)
	{
		REF_CHECK(mutex);
		mutex.LockRead();
	}

	~ScopedReadLock()
	{
		REF_CHECK(mutex);
		mutex.UnlockRead();
	}

private:
	ReadWriteLock& mutex;
};
