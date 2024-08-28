// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "Graphics/GeometryPrimitives.hpp"

struct NativeTexture;

struct BatchElement
{
	DynamicArray<PrimitiveVertex> batchedVertices;
	DynamicArray<u32> batchedIndices;
	const NativeTexture* batchTexture;
	//TODO - clipping must be addressed
	// Either scissor or stencil clipping...?
};

class WidgetBatchElements
{
public:
	NODISCARD BatchElement& GetBatchElement(const NativeTexture* batchTexture);
	NODISCARD forceinline const DynamicArray<BatchElement>& GetBatchElementList() { return batchElements; }

	// TODO - Clipping

private:
	DynamicArray<BatchElement> batchElements;
};

