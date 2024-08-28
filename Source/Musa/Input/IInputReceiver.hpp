// Copyright 2020, Nathan Blane

#pragma once

#include "InputDefinitions.hpp"
#include "Input/InputEvents.hpp"

class IInputReceiver
{
public:
	virtual ~IInputReceiver() = default;

	//virtual InputEvents OnControllerAnalogChange(u32 controllerIndex, Input::Gamepad analogInput, f32 analogValue) = 0;
	virtual InputEvents OnMouseMove(const IntVector2& currentMousePos, const IntVector2& prevMousePos, const IntVector2& delta) = 0;
	virtual InputEvents OnMouseEnter(const IntVector2& currentMousePos, const IntVector2& prevMousePos, const IntVector2& delta) = 0;
	virtual InputEvents OnMouseExit(const IntVector2& currentMousePos, const IntVector2& prevMousePos, const IntVector2& delta) = 0;
	virtual InputEvents OnMouseDown(Input::Buttons input) = 0;
	virtual InputEvents OnMouseUp(Input::Buttons input) = 0;
	virtual InputEvents OnKeyDown(Input::Buttons input, bool isRepeated) = 0;
	virtual InputEvents OnKeyUp(Input::Buttons input) = 0;
	virtual InputEvents OnChar(tchar c, bool isRepeated) = 0;
	virtual InputEvents OnFocusReceived() = 0;
	virtual InputEvents OnFocusLost() = 0;

	virtual InputEvents OnActivationChanged(bool activated) = 0;

	// These functions aren't actually going to be needed because the input system will only contain the current UI path
};
