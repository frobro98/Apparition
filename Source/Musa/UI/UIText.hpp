// Copyright 2020, Nathan Blane

#pragma once

#include "UI/UIWidget.hpp"
#include "String/String.h"
#include "BasicTypes/Color.hpp"
#include "Font/FontCache.hpp"

namespace UI
{
class Text : public Widget
{
public:
	Text(FontID fontID);

	virtual void UpdateInternal() override;
	virtual void PrepareForRenderInternal(WidgetBatchElements& widgetElements) override;

	inline void SetText(StringView sv)
	{
		text = String(*sv);
		// TODO - Inform low level UI of update!
	}

	inline void SetColor(Color c)
	{
		color = c;
		// TODO - Inform low level UI of update!
	}

	inline const tchar* GetText() const
	{
		return *text;
	}

	inline Color GetColor() const
	{
		return color;
	}

	inline Font& GetFont() const
	{
		return *font;
	}


private:
	String text;
	Color color;
	Font* font;
};
}