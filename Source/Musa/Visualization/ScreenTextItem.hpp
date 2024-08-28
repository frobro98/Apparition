// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"
#include "BasicTypes/Color.hpp"
#include "Math/Vector2.hpp"

struct ScreenTextItem
{
	String text;
	Color32 color;
	Vector2 screenPosition;
	f32 scale;
};