// Copyright 2020, Nathan Blane

#include "UIText.hpp"

namespace UI
{
Text::Text(FontID fontID)
	: font(GetLoadedFont(fontID))
{
}

void Text::UpdateInternal()
{
}

void Text::PrepareForRenderInternal(WidgetBatchElements& widgetElements)
{
	UNUSED(widgetElements);
}
}
