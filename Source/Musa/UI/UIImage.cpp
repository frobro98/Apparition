// Copyright 2020, Nathan Blane

#include "UIImage.hpp"
#include "WidgetBatchElemements.hpp"

namespace UI
{
Image::Image(Widget* parent)
	: Widget(parent)
{
}
void Image::UpdateInternal()
{
}
void Image::PrepareForRenderInternal(WidgetBatchElements& widgetElements)
{
	BatchElement& batchElement = widgetElements.GetBatchElement(nullptr);
	
	Vector2 topLeft = absolutePosition;
	Vector2 topRight = Vector2(absolutePosition.x + absoluteScale.x, absolutePosition.y);
	Vector2 bottomLeft = Vector2(absolutePosition.x, absolutePosition.y - absoluteScale.y);
	Vector2 bottomRight = Vector2(absolutePosition.x + absoluteScale.x, absolutePosition.y - absoluteScale.y);

	u32 indexStart = batchElement.batchedVertices.Size();
	// Need to construct a quad in world space
	batchElement.batchedVertices.Add(PrimitiveVertex{
										Vector3(topLeft, 0),	// Position
										Vector2(0, 1),			// Texture Coords
										NormalizeColor(color)	// Color
									});
	batchElement.batchedVertices.Add(PrimitiveVertex{
									Vector3(bottomLeft, 0),	// Position
									Vector2(0, 0),			// Texture Coords
									NormalizeColor(color)	// Color
		});
	batchElement.batchedVertices.Add(PrimitiveVertex{
									Vector3(bottomRight, 0),	// Position
									Vector2(1, 0),			// Texture Coords
									NormalizeColor(color)	// Color
		});
	batchElement.batchedVertices.Add(PrimitiveVertex{
									Vector3(topRight, 0),	// Position
									Vector2(1, 1),			// Texture Coords
									NormalizeColor(color)	// Color
		});

	batchElement.batchedIndices.Add(indexStart + 0);
	batchElement.batchedIndices.Add(indexStart + 1);
	batchElement.batchedIndices.Add(indexStart + 2);

	batchElement.batchedIndices.Add(indexStart + 2);
	batchElement.batchedIndices.Add(indexStart + 3);
	batchElement.batchedIndices.Add(indexStart + 0);
}
}