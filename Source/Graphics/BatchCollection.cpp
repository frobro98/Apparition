// Copyright 2020, Nathan Blane

#include "BatchCollection.hpp"
#include "RenderContext.hpp"
#include "ResourceArray.hpp"
#include "ResourceInitializationDescriptions.hpp"

void BatchCollection::BatchLine(const BatchedLineDescription& lineDesc)
{
	PrimitiveVertex vert = {};
	vert.position = lineDesc.start;
	vert.color = lineDesc.color;
	batchedLines.Add(vert);

	vert = {};
	vert.position = lineDesc.end;
	vert.color = lineDesc.color;
	batchedLines.Add(vert);
}

void BatchCollection::BatchLine(const BatchedLineDescription* lineDescs, u32 lineCount)
{
	Assert(lineDescs);
	for (u32 i = 0; i < lineCount; ++i)
	{
		BatchLine(lineDescs[i]);
	}
}

void BatchCollection::BatchTriangle(const BatchedTriangleDescription& triDesc)
{
	PrimitiveVertex v0 = {
	triDesc.pos0,
	Vector2(0, 0),
	triDesc.color
	};
	PrimitiveVertex v1 = {
	triDesc.pos1,
	Vector2(1, 1),
	triDesc.color
	};
	PrimitiveVertex v2 = {
		triDesc.pos2,
		Vector2(0, 1),
		triDesc.color
	};
	batchedTris.Add(PrimitiveTriangle{ v0, v1, v2 });
}

void BatchCollection::BatchQuad(const BatchedQuadDescription& quadDesc)
{
	const float halfWidth = quadDesc.width * .5f;
	const float halfHeight = quadDesc.height * .5f;
	{
		PrimitiveVertex v0 = {
			Vector3(
				quadDesc.position.x - halfWidth,
				quadDesc.position.y - halfHeight,
				quadDesc.position.z),
			Vector2(0, 0), quadDesc.color
		};
		PrimitiveVertex v1 = {
			Vector3(
				quadDesc.position.x + halfWidth,
				quadDesc.position.y + halfHeight,
				quadDesc.position.z),
			Vector2(1, 1), quadDesc.color
		};
		PrimitiveVertex v2 = {
			Vector3(
				quadDesc.position.x - halfWidth,
				quadDesc.position.y + halfHeight,
				quadDesc.position.z),
			Vector2(0, 1), quadDesc.color
		};

		batchedTris.Add(PrimitiveTriangle{ v0, v1, v2 });
	}

	{
		PrimitiveVertex v0 = {
		Vector3(
			quadDesc.position.x - halfWidth,
			quadDesc.position.y - halfHeight,
			quadDesc.position.z),
		Vector2(0, 0), quadDesc.color
		};
		PrimitiveVertex v1 = {
			Vector3(
				quadDesc.position.x + halfWidth,
				quadDesc.position.y - halfHeight,
				quadDesc.position.z),
			Vector2(1, 0), quadDesc.color
		};
		PrimitiveVertex v2 = {
			Vector3(
				quadDesc.position.x + halfWidth,
				quadDesc.position.y + halfHeight,
				quadDesc.position.z),
			Vector2(1, 1), quadDesc.color
		};
		batchedTris.Add(PrimitiveTriangle{ v0, v1, v2 });
	}
}

void BatchCollection::BatchCircle(const BatchedCircleDescription& circleDesc)
{
	const Vector3 center = circleDesc.position;
	const Vector3 up = circleDesc.up;
	const Vector3 right = circleDesc.right;
	const f32 angleIncrement = Math::TwoPi / circleDesc.numSegments;
	const f32 radius = circleDesc.radius;
	const f32 diameter = radius * 2.f;
	f32 delta = 0;

	PrimitiveVertex v0 = {
		center,
		Vector2(.5f, .5f), circleDesc.color
	};

	auto positionCalculation = [&](float delt) {
		return center + (Math::Cos(delt) * right + Math::Sin(delt) * up) * radius;
	};

	for (u32 i = 0; i < circleDesc.numSegments; ++i)
	{
		Vector3 pos = positionCalculation(delta);
		PrimitiveVertex v1 = {
			pos,
			Vector2(
				.5f + (pos.x - center.x) / diameter,
				.5f + (pos.y - center.y) / diameter
			),
			circleDesc.color
		};
		delta += angleIncrement;

		pos = positionCalculation(delta);
		PrimitiveVertex v2 = {
			pos,
			Vector2(
				.5f + (pos.x - center.x) / diameter,
				.5f + (pos.y - center.y) / diameter
			),
			circleDesc.color
		};

		batchedTris.Add(PrimitiveTriangle{ v0, v1, v2 });
	}
}

void BatchCollection::BatchWireTriangle(const BatchedTriangleDescription& triDesc)
{
	BatchedLineDescription line = {};
	line.color = triDesc.color;
	line.start = triDesc.pos0;
	line.end = triDesc.pos1;
	BatchLine(line);

	line.start = triDesc.pos1;
	line.end = triDesc.pos2;
	BatchLine(line);

	line.start = triDesc.pos2;
	line.end = triDesc.pos0;
	BatchLine(line);
}

void BatchCollection::BatchWireQuad(const BatchedQuadDescription& quadDesc)
{
	float halfWidth = quadDesc.width * .5f;
	float halfHeight = quadDesc.height * .5f;

	Vector3 v0 = Vector3(
		quadDesc.position.x - halfWidth,
		quadDesc.position.y - halfHeight,
		quadDesc.position.z);
	Vector3 v1 = Vector3(
		quadDesc.position.x - halfWidth,
		quadDesc.position.y + halfHeight,
		quadDesc.position.z);
	Vector3 v2 = Vector3(
		quadDesc.position.x + halfWidth,
		quadDesc.position.y + halfHeight,
		quadDesc.position.z);
	Vector3 v3 = Vector3(
		quadDesc.position.x + halfWidth,
		quadDesc.position.y - halfHeight,
		quadDesc.position.z);

	BatchedLineDescription line = {};
	line.color = quadDesc.color;
	line.start = v0;
	line.end = v1;
	BatchLine(line);

	line.start = v1;
	line.end = v2;
	BatchLine(line);

	line.start = v2;
	line.end = v3;
	BatchLine(line);

	line.start = v3;
	line.end = v0;
	BatchLine(line);
}

void BatchCollection::BatchWireCircle(const BatchedCircleDescription& circleDesc)
{
	const Vector3 center = circleDesc.position;
	const Vector3 up = circleDesc.up;
	const Vector3 right = circleDesc.right;
	const f32 angleIncrement = Math::TwoPi / circleDesc.numSegments;
	const f32 radius = circleDesc.radius;
	f32 delta = 0;

	auto positionCalculation = [&](float delt) {
		return center + (Math::Cos(delt) * right + Math::Sin(delt) * up) * radius;
	};
	Vector3 prevVertPos = positionCalculation(delta);
	delta += angleIncrement;

	BatchedLineDescription line = {};
	line.color = circleDesc.color;

	for (u32 i = 0; i < circleDesc.numSegments; ++i)
	{
		Vector3 currVertPos = positionCalculation(delta);
		line.start = prevVertPos;
		line.end = currVertPos;
		BatchLine(line);

		delta += angleIncrement;
		prevVertPos = currVertPos;
	}
}

void BatchCollection::RenderBatches(RenderContext& renderer, const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader, const NativeUniformBuffer& viewBuffer, const NativeTexture& texture)
{
	RenderLineBatches(renderer, vertShader, fragShader, viewBuffer, texture);
	RenderTriangleBatches(renderer, vertShader, fragShader, viewBuffer, texture);

	batchedLines.Clear();
	batchedTris.Clear();
}

void BatchCollection::RenderLineBatches(RenderContext& renderer, const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader, const NativeUniformBuffer& viewBuffer, const NativeTexture& texture) const
{
	if (HasLineBatches())
	{
		GraphicsPipelineDescription pipelineDesc = {};
		renderer.InitializeWithRenderState(pipelineDesc);
		pipelineDesc.vertexShader = &vertShader;
		pipelineDesc.fragmentShader = &fragShader;
		pipelineDesc.blendingDescs[0] = BlendDesc();
		pipelineDesc.topology = PrimitiveTopology::TriangleList;
		pipelineDesc.depthStencilTestDesc = DepthTestDesc();
		pipelineDesc.rasterizerDesc = RasterDesc<FillMode::Full, CullingMode::None>();
		pipelineDesc.vertexInputs = GetVertexInput<PrimitiveVertex>();
		renderer.SetGraphicsPipeline(pipelineDesc);

		renderer.SetUniformBuffer(viewBuffer, 0);
		renderer.SetTexture(texture, *SamplerDesc(), 1);

		renderer.DrawRaw(batchedLines, 1);
	}
}

void BatchCollection::RenderTriangleBatches(RenderContext& renderer, const NativeVertexShader& vertShader, const NativeFragmentShader& fragShader, const NativeUniformBuffer& viewBuffer, const NativeTexture& texture) const
{
	if (HasTriangleBatches())
	{
		u32 vertCount = batchedTris.Size() * 3;
		DynamicArray<PrimitiveVertex> totalTriVerts;
		totalTriVerts.Reserve(vertCount);
		for (u32 i = 0; i < batchedTris.Size(); ++i)
		{
			const PrimitiveTriangle& tri = batchedTris[i];
			totalTriVerts.Add(tri.v0);
			totalTriVerts.Add(tri.v1);
			totalTriVerts.Add(tri.v2);
		}

		GraphicsPipelineDescription pipelineDesc = {};
		renderer.InitializeWithRenderState(pipelineDesc);
		pipelineDesc.vertexShader = &vertShader;
		pipelineDesc.fragmentShader = &fragShader;
		pipelineDesc.blendingDescs[0] = BlendDesc();
		pipelineDesc.topology = PrimitiveTopology::TriangleList;
		pipelineDesc.depthStencilTestDesc = DepthTestDesc<CompareOperation::None, false>();
		pipelineDesc.rasterizerDesc = RasterDesc<FillMode::Full, CullingMode::None>();
		pipelineDesc.vertexInputs = GetVertexInput<PrimitiveVertex>();
		renderer.SetGraphicsPipeline(pipelineDesc);

		renderer.SetUniformBuffer(viewBuffer, 0);
		renderer.SetTexture(texture, *SamplerDesc(), 1);

		renderer.DrawRaw(totalTriVerts, 1);
	}
}
