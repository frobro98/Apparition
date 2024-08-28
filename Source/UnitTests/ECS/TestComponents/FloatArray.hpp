// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "ECS/Component.hpp"

// NOTE: This definitely isn't a good kind of component! It doesn't describe what the data actually is and 
// there might be at least 2 different systems that need a float array with the same N size. What happens then???

struct FloatArray : Musa::Component
{
	f32 array[64];
};

