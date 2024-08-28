// Copyright 2020, Nathan Blane

#include "InputContext.hpp"

InputContext& MakeInputContext(const StringView& name)
{
	InputContext* context = new InputContext;
	context->id.hash = GetHash(name);
	return *context;
}

bool ContainsInputAsAction(Input::Buttons button, const InputContext& context)
{
	return context.inputActions.FindFirstIndexUsing([&](const SingleInput& input) {
		return input.type == button;
	}) >= 0;
}

bool ContainsInputAsState(Input::Buttons button, const InputContext& context)
{
	return context.inputStates.FindFirstIndexUsing([&](const SingleInput& input) {
		return input.type == button;
	}) >= 0;
}

RangedInput* GetInputRange(Input::Buttons button, InputContext& context)
{
	return context.inputRanges.FindFirstUsing([&](const RangedInput& input) {
		return input.input.type == button;
	});
}
