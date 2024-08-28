#pragma once

#include "BasicTypes/Intrinsics.hpp"

class IThreadExecution
{
public:
	virtual ~IThreadExecution() {}

	virtual void ThreadInit() {}
	virtual void ThreadBody() = 0;
	virtual void RequestStop() = 0;
	virtual void ThreadExit() {}

};
