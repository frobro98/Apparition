// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector3.hpp"
#include "BasicTypes/Color.hpp"
#include "Containers/DynamicArray.hpp"
#include "GeometryPrimitives.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Graphics/GraphicsAPI.hpp"

class RenderContext;
struct NativeUniformBuffer;
struct NativeTexture;

struct BatchedLineDescription
{
	Vector3 start;
	Vector3 end;
	Color32 color;
};

struct BatchedTriangleDescription
{
	Color32 color;
	Vector3 pos0;
	Vector3 pos1;
	Vector3 pos2;
};

struct BatchedQuadDescription
{
	Color32 color;
	Vector3 position;
	float width;
	float height;
};

struct BatchedCircleDescription
{
	Color32 color;
	Vector3 position;
	Vector3 up;
	Vector3 right;
	float radius;
	float numSegments;
};

class GRAPHICS_API BatchCollection
{
public:
	void BatchLine(const BatchedLineDescription& lineDesc);
	void BatchLine(const BatchedLineDescription* lineDescs, u32 lineCount);

	void BatchTriangle(const BatchedTriangleDescription& triDesc);
	void BatchQuad(const BatchedQuadDescription& quadDesc);
	void BatchCircle(const BatchedCircleDescription& circleDesc);

	void BatchWireTriangle(const BatchedTriangleDescription& triDesc);
	void BatchWireQuad(const BatchedQuadDescription& quadDesc);
	void BatchWireCircle(const BatchedCircleDescription& circleDesc);
	
	// TODO - Graphics pipeline will currently be set in this function. There isn't really a way for customization for depthstencil, blending, etc. This would be a good thing to investigate
	void RenderBatches(RenderContext& renderer, const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader, const NativeUniformBuffer& viewBuffer, const NativeTexture& texture);

	inline bool HasLineBatches() const { return !batchedLines.IsEmpty(); }
	inline bool HasTriangleBatches() const { return !batchedTris.IsEmpty(); }

private:
	void RenderLineBatches(RenderContext& renderer, const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader, const NativeUniformBuffer& viewBuffer, const NativeTexture& texture) const;
	void RenderTriangleBatches(RenderContext& renderer, const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader, const NativeUniformBuffer& viewBuffer, const NativeTexture& texture) const;

private:
	struct PrimitiveTriangle
	{
		PrimitiveVertex v0;
		PrimitiveVertex v1;
		PrimitiveVertex v2;
	};

	using LineList = DynamicArray<PrimitiveVertex>;
	using TriList = DynamicArray<PrimitiveTriangle>;

	LineList batchedLines;
	TriList batchedTris;

};

