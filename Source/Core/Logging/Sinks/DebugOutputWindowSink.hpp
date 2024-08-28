#pragma once

#include "Logging/LogSink.hpp"

// This sink pushes logs directly to the output debug window in Visual Studio
class CORE_API DebugOutputWindowSink : public LogSink
{
public:
	virtual void OutputFormattedString(const LogLineEntry& entry) override;
private:

};

