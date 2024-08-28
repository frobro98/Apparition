// Copyright 2020, Nathan Blane

#pragma once

#include "UI/UIWidget.hpp"
#include "String/String.h"

namespace UI
{
struct TextInput : public Widget
{
	TextInput() = default;
	TextInput(Widget* parent);

	virtual InputEvents OnChar(tchar character, bool isRepeated) override;

	String inputText;
	// Draw location for text as well as where the text will be next inserted
	u32 cursorLocation;

	virtual void UpdateInternal() override;
	virtual void PrepareForRenderInternal(WidgetBatchElements& widgetElements) override;
};
}

