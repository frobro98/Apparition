// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Containers/DynamicArray.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Graphics/GraphicsAPIDefinitions.hpp"
#include "Shader/ShaderStages.hpp"

class ShaderResource;
struct MaterialResourceTable;
struct PrimUniformBuffer;
struct MaterialProperties;


struct PipelineConfiguration
{
	ShaderResource* shaders[ShaderStage::GfxStageCount];
	ShadingModel shadingModel;
	CullingMode cullMode;
	FillMode fillMode;
	BlendMode blendMode;
};

struct MaterialPrimitive
{
	NativeUniformBuffer* primitiveBuffer;
	NativeUniformBuffer* materialProperties;
	NativeVertexBuffer* vertexBuffer;
	NativeIndexBuffer* indexBuffer;
	const MaterialResourceTable* materialResources;
	PipelineConfiguration pipelineConfig;
};

using MaterialPrimitiveBatch = DynamicArray<MaterialPrimitive>;

class MaterialPrimitiveBatches
{
public:

	// Batches need to include data that lives on components already. It cannot include ownership of that data, sadly.
	// Therefore, the system that controls the rendering will have to set up all of those uniform buffers
	void AddMaterialBatch(const MaterialPrimitiveBatch& primBatch);

	// Get Collection
	forceinline const DynamicArray<MaterialPrimitiveBatch>& GetBatches() { return batches; }

private:
	DynamicArray<MaterialPrimitiveBatch> batches;
};
