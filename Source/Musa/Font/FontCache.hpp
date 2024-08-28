// Copyright 2020, Nathan Blane

#pragma once

#include "Path/Path.hpp"
#include "String/StringView.hpp"
#include "Containers/Map.h"
#include "Math/Vector2.hpp"

class Texture;

// Font Definitions

struct FontID
{
	FontID(const StringView& fontName);

	friend bool operator==(const FontID& id0, const FontID& id1)
	{
		return id0.id == id1.id;
	}

	const u32 id;
};

inline u32 GetHash(FontID id)
{
	return id.id;
}

struct FontCharDescription
{
	// NOTE: These float values are normalized, then scaled up to be around 1 
	u32 width;
	u32 height;
	u32 advance;	
	// Used for chars like g
	u32 characterHeightOffset;

	Vector2 normTextureCoords;
	f32 normCharacterWidth;
	f32 normCharacterHeight;

	tchar characterCode;
};

struct Font
{
	Font(const String& fontName);
	~Font();

	Map<tchar, FontCharDescription> fontCharacterMap;
	Texture* fontTexture = nullptr;
	String name;
	// Height applied to the y offset of the text when encountering newlines
	u32 newlineHeightOffset = 0;
	FontID id;
};

// Import ttf
Font* ImportTTFont(const Path& path);

// Get Cached Font
Font* GetLoadedFont(FontID id);

