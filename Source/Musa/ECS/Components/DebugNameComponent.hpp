// Copyright 2020, Nathan Blane

#pragma once

#include "ECS/Component.hpp"

struct DebugNameComponent : Musa::Component
{
	// TODO - There needs to be a name here, and what better to have this be a compile time/hashed string name thing! That way
	// it would be trivially copyable
};
