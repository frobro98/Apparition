// Copyright 2020, Nathan Blane

#include "ApplicationEventRouter.hpp"
#include "Application/MusaApp.hpp"
#include "Input/InputEvents.hpp"

DEFINE_LOG_CHANNEL(KeyLogging);

void ApplicationEventRouter::HandleKeyDown(Input::Buttons key, const Input::DownState& input, bool isRepeated)
{
	Input::ButtonEvent event;
	event.button = key;
	event.buttonState = Input::ButtonState::Pressed;
	event.downState = input;
	event.isRepeated = isRepeated;
	
	Input::Event inputEvent = {};
	inputEvent.buttonEvent = event;
	inputEvent.type = Input::EventType::Button;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleKeyUp(Input::Buttons key, const Input::DownState& input)
{
	Input::ButtonEvent event;
	event.button = key;
	event.buttonState = Input::ButtonState::Released;
	event.downState = input;
	event.isRepeated = false;
	
	Input::Event inputEvent = {};
	inputEvent.buttonEvent = event;
	inputEvent.type = Input::EventType::Button;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleKeyChar(tchar c, bool isRepeated)
{
	UNUSED(c, isRepeated);
}

void ApplicationEventRouter::HandleMouseDown(Input::Buttons mouse, const Input::DownState& state)
{
	Input::ButtonEvent event;
	event.button = mouse;
	event.buttonState = Input::ButtonState::Pressed;
	event.downState = state;
	event.isRepeated = false;
	
	Input::Event inputEvent = {};
	inputEvent.buttonEvent = event;
	inputEvent.type = Input::EventType::Button;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleMouseUp(Input::Buttons mouse, const Input::DownState& state)
{
	Input::ButtonEvent event;
	event.button = mouse;
	event.buttonState = Input::ButtonState::Released;
	event.downState = state;
	event.isRepeated = false;
	
	Input::Event inputEvent = {};
	inputEvent.buttonEvent = event;
	inputEvent.type = Input::EventType::Button;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleMouseMove(const IntVector2& screenSpaceMousePos, const IntVector2& clientMousePos)
{
	if (screenSpaceMousePos != previousScreenSpacePosition)
	{
		Input::MouseEvent event;
		event.currentScreenSpacePosition = screenSpaceMousePos;
		event.currentClientPosition = clientMousePos;
		event.previousScreenSpacePosition = previousScreenSpacePosition;
		event.previousClientPosition = previousClientPosition;
		event.deltaPosition = screenSpaceMousePos - previousScreenSpacePosition;

		previousScreenSpacePosition = screenSpaceMousePos;
		previousClientPosition = clientMousePos;

		Input::Event inputEvent = {};
		inputEvent.mouseEvent = event;
		inputEvent.type = Input::EventType::Mouse;
		inputEvents.Add(inputEvent);
	}
}

void ApplicationEventRouter::HandleRawMouseMove(const IntVector2& screenSpaceMousePos, const IntVector2& clientMousePos, const IntVector2& deltaPosition)
{
	if (screenSpaceMousePos != previousScreenSpacePosition)
	{
		Input::MouseEvent event;
		event.currentScreenSpacePosition = screenSpaceMousePos;
		event.currentClientPosition = clientMousePos;
		event.previousScreenSpacePosition = previousScreenSpacePosition;
		event.previousClientPosition = previousClientPosition;
		event.deltaPosition = deltaPosition;

		previousScreenSpacePosition = screenSpaceMousePos;
		previousClientPosition = clientMousePos;
		
		Input::Event inputEvent = {};
		inputEvent.mouseEvent = event;
		inputEvent.type = Input::EventType::Mouse;
		inputEvents.Add(inputEvent);
	}
}

void ApplicationEventRouter::HandleMouseScrollWheel(const IntVector2& screenSpaceMousePos, f32 wheelDelta)
{
	Input::MouseEvent event;
	event.currentScreenSpacePosition = screenSpaceMousePos;
	event.currentClientPosition = screenSpaceMousePos;
	event.previousScreenSpacePosition = screenSpaceMousePos;
	event.previousClientPosition = screenSpaceMousePos;
	event.deltaPosition = IntVector2::Zero;
	event.scrollDelta = wheelDelta;

	Input::Event inputEvent = {};
	inputEvent.mouseEvent = event;
	inputEvent.type = Input::EventType::Mouse;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleControllerButtonDown(u32 /*controllerIndex*/, Input::Buttons gamepadInput, const Input::DownState& state)
{
	Input::ButtonEvent event;
	event.button = gamepadInput;
	event.downState = state;
	event.isRepeated = false;
	event.buttonState = Input::ButtonState::Pressed;
	
	Input::Event inputEvent = {};
	inputEvent.buttonEvent = event;
	inputEvent.type = Input::EventType::Button;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleControllerButtonUp(u32 /*controllerIndex*/, Input::Buttons gamepadInput, const Input::DownState& state)
{
	Input::ButtonEvent event;
	event.button = gamepadInput;
	event.downState = state;
	event.isRepeated = false;
	event.buttonState = Input::ButtonState::Pressed;

	Input::Event inputEvent = {};
	inputEvent.buttonEvent = event;
	inputEvent.type = Input::EventType::Button;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleControllerAnalogChange(u32 /*controllerIndex*/, Input::Buttons gamepadInput, f32 normValue)
{
	Input::AnalogEvent event;
	event.analogButton = gamepadInput;
	event.normValue = normValue;

	Input::Event inputEvent = {};
	inputEvent.analogEvent = event;
	inputEvent.type = Input::EventType::Analog;
	inputEvents.Add(inputEvent);
}

void ApplicationEventRouter::HandleWindowResizeEvent(const IntVector2& newWindowdimensions)
{
	Musa::ApplicationEvent event;
	event.type = Musa::ApplicationEventType::Resize;
	event.resizeData = { newWindowdimensions.x, newWindowdimensions.y };

	applicationEvents.Add(event);
}

void ApplicationEventRouter::HandleWindowCloseEvent()
{
	Musa::ApplicationEvent event;
	event.type = Musa::ApplicationEventType::Close;

	applicationEvents.Add(event);
}

void ApplicationEventRouter::HandleWindowActivationChanged(bool activated)
{
	Musa::ApplicationEvent event;
	event.type = activated ? Musa::ApplicationEventType::Activated : Musa::ApplicationEventType::Deactivated;

	applicationEvents.Add(event);
}

DynamicArray<Musa::ApplicationEvent> ApplicationEventRouter::MoveApplicationEvents()
{
	DynamicArray<Musa::ApplicationEvent> events = applicationEvents;
	applicationEvents.Clear();

	return events;
}

DynamicArray<Input::Event> ApplicationEventRouter::MoveFrameInputs()
{
	DynamicArray<Input::Event> events = inputEvents;
	inputEvents.Clear();

	return events;
}
