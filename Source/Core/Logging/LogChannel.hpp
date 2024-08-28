#pragma once

struct CORE_API LogChannel
{
	LogChannel(const tchar* name) : logName(name) {}

	// TODO - Need to move to a compile time/"Name" system for strings
	const tchar* logName;
	// TODO - Figure out if I need anything else to allow for log filtering down the line

};

#define DEFINE_LOG_CHANNEL(SystemName)									\
	inline struct SystemName##LogSlot : LogChannel			\
	{															\
		SystemName##LogSlot() : LogChannel(#SystemName) {}	\
	} SystemName

