// Copyright 2020, Nathan Blane

#pragma once

#pragma once

#include "BasicTypes/Intrinsics.hpp"
// TODO - Remove this somehow
#include "Platform/PlatformDefinitions.h"
#include "CoreAPI.hpp"

// TODO: Decouple OS specific methods from interface

class CORE_API EngineTick
{
public:
	EngineTick();
	~EngineTick();

	void Start();
	void Lap();

	f64 GetSeconds() const;
	f64 GetMilliseconds() const;
	f64 GetMicroseconds() const;

private:
	static float CountsPerSec;

	u64 startTime{};
	u64 deltaTime{};
};