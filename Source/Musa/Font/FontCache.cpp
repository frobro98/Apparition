// Copyright 2020, Nathan Blane

#include "FontCache.hpp"


#include "freetype/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_ADVANCES_H

#include "Texture/Texture2D/TextureManager.h"
#include "Texture/Texture2D/Texture.h"

#include "Math/MathFunctions.hpp"
#include "Containers/MemoryBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// FontID
//////////////////////////////////////////////////////////////////////////

FontID::FontID(const StringView& fontName)
	: id(GetHash(fontName))
{
}

//////////////////////////////////////////////////////////////////////////
// FontCharDescription
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Font
//////////////////////////////////////////////////////////////////////////
Font::Font(const String& fontName)
	: name(fontName),
	id(StringView{ *fontName })
{
}

Font::~Font()
{

}

//////////////////////////////////////////////////////////////////////////
// Font Caching
//////////////////////////////////////////////////////////////////////////

Map<FontID, Font*> importedFonts;

static constexpr u32 ftToPixelSpace(u32 fontSpaceVal)
{
	return fontSpaceVal >> 6;
}

static constexpr u32 ftToFontSpace(u32 pixelSpaceVal)
{
	return pixelSpaceVal << 6;
}

Font* ImportTTFont(const Path& path)
{
	// Ascii only
	constexpr u32 charCount = 256;

	FT_Library lib;
	FT_Error error = FT_Init_FreeType(&lib);
	Assert(error == FT_Err_Ok);

	FT_Face face;
	error = FT_New_Face(lib, path.GetString(), 0, &face);
	Assert(error != FT_Err_Unknown_File_Format);

	constexpr u32 bitmapWidth = 1024;
	constexpr u32 bitmapHeight = 1024;
	constexpr u32 bitmapChannels = 4;
	constexpr u32 sdfScaleFactor = 1;
	constexpr u32 paddingX = 5;
	constexpr u32 paddingY = 5;

	String fontName = "Ariel";
	Font* font = new Font(fontName);

	MemoryBuffer bitmap(bitmapWidth*bitmapHeight*bitmapChannels);
	const u32 sdfHeight = 64 * sdfScaleFactor; // size 16 font with sdf scale
	error = FT_Set_Char_Size(face, 0, ftToFontSpace(sdfHeight), 96, 96); // Everything it seems to be in 1/64 units, so we are shifting right to mult by 64

	u32 acenderInPixels = ftToPixelSpace(FT_MulFix(face->ascender, face->size->metrics.y_scale));

	Texture* t = new Texture;
	t->name = fontName;
	t->format = ImageFormat::RGBA_8norm;

	u32 bitmapWidthOffset = 0;
	u32 bitmapHeightOffset = 0;
	u32 maxCharHeight = 0;
	for (u32 i = 0; i < charCount; ++i)
	{
		tchar c = (tchar)i;

		u32 glyphIndex = FT_Get_Char_Index(face, c);
		if (glyphIndex != 0)
		{
			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);

			FT_GlyphSlot glyph = face->glyph;
			FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);

			// Transfer bitmap information to font cache

			// This is actually how much the draw position should advance, not the width of the glyph
			u32 glyphWidth = glyph->bitmap.width;
			u32 advance = ftToPixelSpace(glyph->advance.x); // Why do we shift left 6?? Everything it seems to be in 1/64 units, so we are dividing by 64
			glyphWidth *= sdfScaleFactor;
			u32 glyphHeight = glyph->bitmap.rows;
			glyphHeight *= sdfScaleFactor;

			// Move to next row
			if (bitmapWidthOffset + glyphWidth > bitmapWidth)
			{
				bitmapWidthOffset = 0;
				bitmapHeightOffset += maxCharHeight + paddingY;
				maxCharHeight = 0;
			}

			Assert(bitmapHeightOffset + glyphHeight < bitmapHeight);

			maxCharHeight = Math::Max(maxCharHeight, glyphHeight);

			float invBitmapWidth = 1.f / bitmapWidth;
			float invBitmapHeight = 1.f / bitmapHeight;

			FontCharDescription desc = {};
			desc.characterCode = c;
			desc.normTextureCoords = Vector2((float)bitmapWidthOffset * invBitmapWidth,  (float)bitmapHeightOffset * invBitmapHeight);

			desc.width = glyphWidth;
			desc.height = glyphHeight;
			desc.advance = advance;
			desc.normCharacterWidth = glyphWidth * invBitmapWidth;
			desc.normCharacterHeight = glyphHeight * invBitmapHeight;
			desc.characterHeightOffset = (acenderInPixels - glyph->bitmap_top);

			font->fontCharacterMap.Add(c, desc);

			u32 bitmapY = glyph->bitmap.rows - 1;
			for (u32 y = 0; y < glyph->bitmap.rows; ++y)
			{
				for (u32 x = 0; x < glyph->bitmap.width; ++x)
				{
					const u8 bitmapVal = glyph->bitmap.buffer[x + bitmapY * glyph->bitmap.width];
					const u32 xTexPos = x + bitmapWidthOffset;
					const u32 yTexPos = y + bitmapHeightOffset;
					const u32 totalArrayPos = (xTexPos + yTexPos * bitmapWidth) * 4;

					// TODO - Because of blending, there needs to be some meaningful alpha value. 
					// TODO - The alpha lives in the bitmap, so it doesn't really make much sense to store it multiple times. Just keep the alpha for the entire pixel, disregarding the other 3 values
					*bitmap.Offset(totalArrayPos + 0) = bitmapVal; // red
					*bitmap.Offset(totalArrayPos + 1) = bitmapVal; // green
					*bitmap.Offset(totalArrayPos + 2) = bitmapVal; // blue
					*bitmap.Offset(totalArrayPos + 3) = bitmapVal; // Alpha; TODO - put sdf information in this...
				}
				--bitmapY;
			}

			bitmapWidthOffset += glyphWidth + paddingX;

		}
	}

	importedFonts.Add(font->id, font);

	MipmapLevel level;
	level.width = bitmapWidth;
	level.height = bitmapHeight;
	level.mipData = { bitmap.GetData(), bitmap.Size() };
	t->mipLevels.Add(MOVE(level));

	t->UpdateNativeResources();

	font->fontTexture = t;
	font->newlineHeightOffset = maxCharHeight;

	GetTextureManager().AddTexture(*t);

	error = FT_Done_FreeType(lib);

	return font;
}

Font* GetLoadedFont(FontID id)
{
	Font** font = importedFonts.Find(id);
	return font != nullptr ? *font : nullptr;
}

