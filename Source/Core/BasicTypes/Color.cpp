// Copyright 2020, Nathan Blane

#include "Color.hpp"

Color32 NormalizeColor(const Color& color)
{
	return Color32(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

Color UnnormalizeColor(const Color32& color)
{
	u8 r = static_cast<u8>(color.r * 255);
	u8 g = static_cast<u8>(color.g * 255);
	u8 b = static_cast<u8>(color.b * 255);
	u8 a = static_cast<u8>(color.a * 255);
	return Color(r, g, b, a);
}
