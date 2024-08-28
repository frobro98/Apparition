// Copyright 2020, Nathan Blane

#include "UIConsole.hpp"
#include "Font/FontCache.hpp"
#include "Texture/Texture2D/Texture.h"
#include "UI/WidgetBatchElemements.hpp"

namespace UI
{
Console::Console()
	: background(MakeUnique<Image>(this))
{
	background->color = Color::Black();
	background->color.a /= 2;
	relativeScale = Vector2(10, 50);

	InitializeFont();
}
Console::Console(Widget* parent)
	: Widget(parent),
	//consoleInput(MakeUnique<TextInput>(this)),
	background(MakeUnique<Image>(this))
{
	textOnScreen = "_";
	InitializeFont();
}

InputEvents Console::OnKeyDown(Input::Buttons keyType, bool /*isRepeated*/)
{
	// Handling delete/backspace/tab/arrow key movements

	if (keyType == Input::Key_Tilde)
	{

	}

	return InputEvents();
}

InputEvents Console::OnChar(tchar c, bool /*isRepeated*/)
{
	constexpr tchar minChar = 32;
	constexpr tchar maxChar = 127;
	if (c >= minChar && c < maxChar)
	{
		textOnScreen += c;
		return InputEvents(Handled);
	}
	//InputEvents events = consoleInput->OnChar(c, isRepeated);
	return InputEvents();
}

void Console::UpdateInternal()
{
	//consoleInput->Update(relativePosition, relativeScale);
	background->Update(relativePosition, relativeScale);
}

void Console::PrepareForRenderInternal(WidgetBatchElements& widgetElements)
{
	//BatchElement& elem = widgetElements.GetBatchElement(nullptr);
	background->PrepareForRenderInternal(widgetElements);

	Texture& fontTex = *consoleFont->fontTexture;
	TextureResource& res = fontTex.GetResource();
	BatchElement& batchElement = widgetElements.GetBatchElement(res.texResource);

	u32 textSize = textOnScreen.Length();
	Vector2 currentTextPosition;
	constexpr tchar spaceCode = 0x20;
	for (u32 i = 0; i < textSize; ++i)
	{
		tchar character = textOnScreen[i];
		FontCharDescription* charDesc = consoleFont->fontCharacterMap.Find(character);
		if (charDesc->characterCode != spaceCode)
		{
			//const u32 texWidth = fontTex.GetWidth();
			//const u32 texHeight = fontTex.GetHeight();

			//const float halfWidth = charDesc->width * /*textScale **/ .5f;
			//const float halfHeight = charDesc->height * /*textScale **/ .5f;

			const float negX = currentTextPosition.x;
			const float posY = currentTextPosition.y - (charDesc->characterHeightOffset/* * textScale*/);
			const float posX = negX + (charDesc->width/* * textScale*/);
			const float negY = posY - (charDesc->height/* * textScale*/);

			const float uNormCoord = charDesc->normTextureCoords.x;
			const float vNormCoord = charDesc->normTextureCoords.y;
			const float uNormSize = charDesc->normCharacterWidth;
			const float vNormSize = charDesc->normCharacterHeight;

			Color32 white = Color32::White();
			DynamicArray<PrimitiveVertex>& vertices = batchElement.batchedVertices;
			const u32 startIndex = vertices.Size();
			vertices.Add(PrimitiveVertex{
						Vector3(negX, posY, 0),
						Vector2(uNormCoord, vNormCoord + vNormSize),
						white
				});
			vertices.Add(PrimitiveVertex{
				Vector3(negX, negY, 0),
				Vector2(uNormCoord, vNormCoord),
				white
				});
			vertices.Add(PrimitiveVertex{
				Vector3(posX, posY, 0),
				Vector2(uNormCoord + uNormSize, vNormCoord + vNormSize),
				white
				});
			vertices.Add(PrimitiveVertex{
				Vector3(posX, negY, 0),
				Vector2(uNormCoord + uNormSize, vNormCoord),
				white
				});

			DynamicArray<u32>& indices = batchElement.batchedIndices;
			indices.Add(startIndex + 0);
			indices.Add(startIndex + 1);
			indices.Add(startIndex + 2);

			indices.Add(startIndex + 2);
			indices.Add(startIndex + 1);
			indices.Add(startIndex + 3);
		}

		currentTextPosition.x += (charDesc->advance /** textScale*/);
	}

}
void Console::InitializeFont()
{
	FontID fontID = StringView("Ariel");
	consoleFont = GetLoadedFont(fontID);
}
}
