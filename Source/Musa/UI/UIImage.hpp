// Copyright 2020, Nathan Blane

#pragma once

// TODO - UI::Image is not a good description for what this is. It should be able to be an image/texture and/or a solid color value
// Both UE4 and Unity have an Image UI component, essentially boiling down to this behavior, but it seems weird to associate it with an image...

#include "UI/UIWidget.hpp"
#include "BasicTypes/Color.hpp"

namespace UI
{
struct Image : public Widget
{
	Image() = default;
	Image(Widget* parent);

	virtual void UpdateInternal() override;
	virtual void PrepareForRenderInternal(WidgetBatchElements& widgetElements) override;

	Color color = Color::White();
	// Texture...???
	u32 width = 0;
	u32 height = 0;
};
}
