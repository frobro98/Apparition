// Copyright 2020, Nathan Blane

#pragma once

#include "ECS/Component.hpp"
#include "Math/Vector4.hpp"

struct Movement : Musa::Component
{
	Vector4 velocity;
};

