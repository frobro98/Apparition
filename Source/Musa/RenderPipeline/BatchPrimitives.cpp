// Copyright 2020, Nathan Blane

#include "BatchPrimitives.hpp"
#include "Graphics/RenderContext.hpp"
#include "Scene/ScreenView.hpp"
#include "Texture/Texture2D/Texture.h"
#include "Graphics/ResourceInitializationDescriptions.hpp"

static BatchCollection collection;


void BatchLinePrimitive(const BatchedLineDescription& lineDesc)
{
	collection.BatchLine(lineDesc);
}

void BatchLinePrimitives(const BatchedLineDescription* lineDesc, u32 lineCount)
{
	collection.BatchLine(lineDesc, lineCount);
}

void BatchTrianglePrimitive(const BatchedTriangleDescription& triDesc)
{
	collection.BatchTriangle(triDesc);
}

void BatchQuadPrimitive(const BatchedQuadDescription& quadDesc)
{
	collection.BatchQuad(quadDesc);
}

void BatchCirclePrimitive(const BatchedCircleDescription& circleDesc)
{
	collection.BatchCircle(circleDesc);
}

void BatchWireTrianglePrimitive(const BatchedTriangleDescription& triDesc)
{	
	collection.BatchWireTriangle(triDesc);
}

void BatchWireQuadPrimitive(const BatchedQuadDescription& quadDesc)
{
	collection.BatchWireQuad(quadDesc);
}

void BatchWireCirclePrimitive(const BatchedCircleDescription & circleDesc)
{
	collection.BatchWireCircle(circleDesc);
}

void RenderBatchedPrimitives(RenderContext& renderer, const View& view, 
	const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader)
{
	collection.RenderBatches(renderer, 
		vertShader, fragShader, *view.viewBuffer, *(WhiteTexture()->GetResource().texResource));
}
