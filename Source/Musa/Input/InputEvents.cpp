// Copyright 2020, Nathan Blane

#include "InputEvents.hpp"

InputEvents::InputEvents(HandledInput)
	: handled(true)
{
}

void InputEvents::ChangeMousePosition(IntVector2 pos)
{
	containsEvents = true;
	changedMousePositoin = pos;
}

void InputEvents::LockCursor(bool lock)
{
	containsEvents = true;
	lockCursor = lock;
}

void InputEvents::ShowCursor(bool show)
{
	showCursor = show;
}
