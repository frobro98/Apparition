// Copyright 2020, Nathan Blane

#pragma once
#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"
#include "String/String.h"
#include "String/StringView.hpp"
#include "Input/InputDefinitions.hpp"
#include "Input/InputContextID.hpp"
#include "Input/InputEvents.hpp"

struct InputRange
{
	f32 minRawRange;
	f32 maxRawRange;
	f32 minNormalizedRange;
	f32 maxNormalizedRange;
};

struct SingleInput
{
	String inputName;
	Input::Buttons type;
};

struct RangedInput
{
	InputRange range;
	SingleInput input;
};

// TODO - Remove
struct InputRangeValue
{
	const SingleInput* input;
	f32 rangeValue;
};

// TODO - The context is a good idea and will eventually exist in file form. When that happens,
// there may be a refactor of the data that exists within the context. An idea for that refactor
// is to, instead of having inputs to check against, have names to check. That way, the input
// event that is being executed in the game is meaningful, not just an input
struct InputContext
{
	// TODO - Only accepts presses essentially. There needs to be a enum based approach to 
// game input
	DynamicArray<SingleInput> inputActions;
	DynamicArray<SingleInput> inputStates;
	// TODO - Ranges aren't really something that makes sense from a usability standpoint. The
	// reason behind this thought is the branching behaviors that come from implementing some
	// input behavior. One example would be character movement. If you want to move a character
	// with a controller, that would be a range input, but if you want to do the same with the
	// keyboard, that would be state input. The user would need to check for, both kinds of input.
	//
	// This is under the impression that we give the user the raw input. This could be solved upon
	// receiving a specific kind of input for processing in ECS. It could be transformed into an 
	// input command or something that is better to digest from the user perspective and represents
	// a gameplay action.
	DynamicArray<RangedInput> inputRanges;
	InputContextID id;

};

forceinline bool operator==(const InputContext& lhs, const InputContext& rhs)
{
	return lhs.id == rhs.id;
}

InputContext& MakeInputContext(const StringView& name);

bool ContainsInputAsAction(Input::Buttons button, const InputContext& context);
bool ContainsInputAsState(Input::Buttons button, const InputContext& context);
// TODO - Not const correct!
RangedInput* GetInputRange(Input::Buttons button, InputContext& context);

