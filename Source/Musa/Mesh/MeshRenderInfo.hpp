// Copyright 2020, Nathan Blane

#pragma once

#include "Math/BoundsVolumes.hpp"
#include "Math/Matrix4.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"

struct MaterialRenderDescription;

// TODO - There needs to be different ones for this and for animated meshes!
struct MeshRenderInfo 
{
	SphereBounds bounds;
	MaterialRenderDescription* meshMaterial = nullptr;
	NativeVertexBuffer* vertexBuffer = nullptr;
	NativeIndexBuffer* indexBuffer = nullptr;
	NativeUniformBuffer* transformBuffer = nullptr;
};
