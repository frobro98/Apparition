#pragma once

#include "String/String.h"
#include "Logging/LogChannel.hpp"
#include "Logging/LogLevel.hpp"

// TODO - This sort of represents a line within the log. This will be changed to 
// show this once logging works
struct LogLineEntry
{
	String logMsg;
	const tchar* logSlot;
	LogLevel::Type level;
};
