// Copyright 2020, Nathan Blane

#include "UITextInput.hpp"
#include "UIText.hpp"

namespace UI
{
TextInput::TextInput(Widget* parent)
	: Widget(parent)
{
}
InputEvents TextInput::OnChar(tchar /*character*/, bool /*isRepeated*/)
{
	return InputEvents();
}
void TextInput::UpdateInternal()
{
}
void TextInput::PrepareForRenderInternal(WidgetBatchElements& widgetElements)
{
	UNUSED(widgetElements);
}
}


