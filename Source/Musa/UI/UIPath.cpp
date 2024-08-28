// Copyright 2020, Nathan Blane

#include "UIPath.hpp"

UI::Path::Path(DynamicArray<Widget*>&& widgetPath)
	: path(MOVE(widgetPath))
{
}

u32 UI::Path::Size() const
{
	return path.Size();
}

bool UI::Path::ContainsWidget(const Widget& widget) const
{
	return path.Contains(&widget);
}
