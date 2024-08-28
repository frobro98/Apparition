// Copyright 2020, Nathan Blane

#pragma once

#include <mutex>
#include <type_traits>

#include "Debugging/Assertion.hpp"

template <typename Resource, typename T>
class ResourceLockPtr;

template <typename Resource, typename = std::enable_if_t<std::is_pointer_v<Resource>>>
class ResourceLockPtr
{
public:
	ResourceLockPtr(Resource res, std::mutex& mut)
		: lockedResource(res),
		mutexRef(mut)
	{
		Assert(res);
		mutexRef.lock();
	}

	~ResourceLockPtr()
	{
		mutexRef.unlock();
	}

	Resource operator->()
	{
		return lockedResource;
	}

private:
	Resource lockedResource;
	std::mutex& mutexRef;
};

