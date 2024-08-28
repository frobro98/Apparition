#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

struct LogLineEntry;

class CORE_API LogSink
{
public:
	virtual void OutputFormattedString(const LogLineEntry& entry) = 0;
};
