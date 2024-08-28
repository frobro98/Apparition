// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Math/IntVector2.hpp"
#include "Input/InputDefinitions.hpp"
#include "UI/UIContext.hpp"
#include "Input/InputEvents.hpp"
#include "Application/ApplicationEvent.hpp"

class MusaApp;

namespace Input
{
struct DownState;
}

// TODO - Rename. Router isn't actually what it does
class ApplicationEventRouter
{
public:
	// Handle Application Events that then get dispatched if needed to the game
	void HandleKeyDown(Input::Buttons key, const Input::DownState& input, bool isRepeated);
	void HandleKeyUp(Input::Buttons key, const Input::DownState& input);
	void HandleKeyChar(tchar c, bool isRepeated);

	void HandleMouseDown(Input::Buttons mouse, const Input::DownState& inputState);
	void HandleMouseUp(Input::Buttons mouse, const Input::DownState& inputState);
	void HandleMouseMove(const IntVector2& screenSpaceMousePos, const IntVector2& clientMousePos);
	void HandleRawMouseMove(const IntVector2& mousePosition, const IntVector2& clientMousePos, const IntVector2& deltaPosition);
	void HandleMouseScrollWheel(const IntVector2& mousePosition, f32 wheelDelta);

	void HandleControllerButtonDown(u32 controllerIndex, Input::Buttons gamepadInput, const Input::DownState& state);
	void HandleControllerButtonUp(u32 controllerIndex, Input::Buttons gamepadInput, const Input::DownState& state);
	void HandleControllerAnalogChange(u32 controllerIndex, Input::Buttons gamepadInput, f32 normValue);

	void HandleWindowResizeEvent(const IntVector2& newWindowdimensions);
	void HandleWindowCloseEvent();
	void HandleWindowActivationChanged(bool activated);

	DynamicArray<Musa::ApplicationEvent> MoveApplicationEvents();
	DynamicArray<Input::Event> MoveFrameInputs();

private:
	DynamicArray<Musa::ApplicationEvent> applicationEvents;
	DynamicArray<Input::Event> inputEvents;
	IntVector2 previousScreenSpacePosition;
	IntVector2 previousClientPosition;
};
