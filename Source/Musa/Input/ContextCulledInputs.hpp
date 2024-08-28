// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "Input/InputDefinitions.hpp"

struct ButtonState
{
	Input::Buttons button;
	f32 stateValue;
};

forceinline bool operator==(const ButtonState& lhs, const ButtonState& rhs)
{
	return lhs.button == rhs.button;
}

struct ContextCulledInputs
{
	DynamicArray<Input::Buttons> actions;
	DynamicArray<ButtonState> states;
};

