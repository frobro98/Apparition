// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

struct CORE_API Color
{
	constexpr Color() = default;
	constexpr Color(u8 red, u8 green, u8 blue)
		: r(red),
		g(green),
		b(blue)
	{
	}
	constexpr Color(u8 red, u8 green, u8 blue, u8 alpha)
		: r(red),
		g(green),
		b(blue),
		a(alpha)
	{
	}

	u8 r = 255;
	u8 g = 255;
	u8 b = 255;
	u8 a = 255;

	static constexpr Color White()
	{
		return Color();
	}
	static constexpr Color LightGray()
	{
		return Color(192, 192, 192);
	}
	static constexpr Color Gray()
	{
		return Color(128, 128, 128);
	}
	static constexpr Color DarkGray()
	{
		return Color(64, 64, 64);
	}
	static constexpr Color Black()
	{
		return Color(0, 0, 0);
	}
	static constexpr Color Red()
	{
		return Color(255, 0, 0);
	}
	static constexpr Color Green()
	{
		return Color(0, 255, 0);
	}
	static constexpr Color Blue()
	{
		return Color(0, 0, 255);
	}
	static constexpr Color Magenta()
	{
		return Color(255, 0, 255);
	}
	static constexpr Color Cyan()
	{
		return Color(0, 255, 255);
	}
	static constexpr Color Yellow()
	{
		return Color(255, 255, 0);
	}
};

struct CORE_API Color32
{
	constexpr Color32() = default;
	constexpr Color32(float red, float green, float blue)
		: r(red),
		g(green),
		b(blue)
	{
	}
	constexpr Color32(float red, float green, float blue, float alpha)
		: r(red),
		g(green),
		b(blue),
		a(alpha)
	{
	}

	float r = 1.f;
	float g = 1.f;
	float b = 1.f;
	float a = 1.f;

	static constexpr Color32 White()
	{
		return Color32();
	}
	static constexpr Color32 LightGray()
	{
		return Color32(.75f, .75f, .75f);
	}
	static constexpr Color32 Gray()
	{
		return Color32(.5f, .5f, .5f);
	}
	static constexpr Color32 DarkGray()
	{
		return Color32(.25f, .25f, .25f);
	}
	static constexpr Color32 Black()
	{
		return Color32(0, 0, 0);
	}
	static constexpr Color32 Red()
	{
		return Color32(1, 0, 0);
	}
	static constexpr Color32 Green()
	{
		return Color32(0, 1, 0);
	}
	static constexpr Color32 Blue()
	{
		return Color32(0, 0, 1);
	}
	static constexpr Color32 Magenta()
	{
		return Color32(1, 0, 1);
	}
	static constexpr Color32 Cyan()
	{
		return Color32(0, 1, 1);
	}
	static constexpr Color32 Yellow()
	{
		return Color32(1, 1, 0);
	}
};

CORE_API Color32 NormalizeColor(const Color& color);
CORE_API Color	UnnormalizeColor(const Color32& color);