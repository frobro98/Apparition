#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/Limits.hpp"

// Synchronization primitive to notify parts of the system about multithreaded events
class ISyncEvent
{
public:
	virtual ~ISyncEvent() {}

	virtual void Create(bool manualReset = false) = 0;
	virtual bool Wait(u32 waitTime = U32Max) = 0;
	virtual void Set() = 0;
	virtual bool Reset() = 0;
	virtual bool IsManualReset() const = 0;
	virtual bool IsValid() const = 0;
};
