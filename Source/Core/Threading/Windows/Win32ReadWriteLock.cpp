
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Threading/ReadWriteLock.hpp"

ReadWriteLock::ReadWriteLock()
{
	::InitializeSRWLock((::PSRWLOCK)&rwLock);
}

void ReadWriteLock::LockRead()
{
	::AcquireSRWLockShared((::PSRWLOCK)&rwLock);
}

void ReadWriteLock::LockWrite()
{
	::AcquireSRWLockExclusive((::PSRWLOCK)&rwLock);
}

void ReadWriteLock::UnlockRead()
{
	::ReleaseSRWLockShared((::PSRWLOCK)&rwLock);
}

void ReadWriteLock::UnlockWrite()
{
	::ReleaseSRWLockExclusive((::PSRWLOCK)&rwLock);
}