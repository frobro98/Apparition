// Copyright 2020, Nathan Blane

#pragma once

#include "Graphics/BatchCollection.hpp"

class RenderContext;
struct View;
struct NativeVertexShader;
struct NativeFragmentShader;

// Define functions to batch specific primitives, like lines, quads, circles, etc

void BatchLinePrimitive(const BatchedLineDescription& lineDesc);
void BatchLinePrimitives(const BatchedLineDescription* lineDesc, u32 lineCount);

void BatchTrianglePrimitive(const BatchedTriangleDescription& triDesc);
void BatchQuadPrimitive(const BatchedQuadDescription& quadDesc);
void BatchCirclePrimitive(const BatchedCircleDescription& circleDesc);

void BatchWireTrianglePrimitive(const BatchedTriangleDescription& triDesc);
void BatchWireQuadPrimitive(const BatchedQuadDescription& quadDesc);
void BatchWireCirclePrimitive(const BatchedCircleDescription& circleDesc);


// Render current batch of primitives
void RenderBatchedPrimitives(RenderContext& renderer, const View& view, 
	const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader);

