// Copyright 2020, Nathan Blane

#include "UIButton.hpp"

namespace UI
{
InputEvents Button::OnMouseDown(Input::Buttons /*input*/)
{
	if (buttonClick)
	{
		buttonClick(*this);
	}
	return InputEvents();
}
InputEvents Button::OnMouseUp(Input::Buttons /*input*/)
{
	return InputEvents();
}
InputEvents Button::OnMouseEnter(const IntVector2& /*currentMousePos*/, const IntVector2& /*prevMousePos*/, const IntVector2& /*delta*/)
{
	if (buttonHover)
	{
		buttonHover(*this);
	}
	return InputEvents();
}
InputEvents Button::OnMouseExit(const IntVector2& /*currentMousePos*/, const IntVector2& /*prevMousePos*/, const IntVector2& /*delta*/)
{
	return InputEvents();
}
}