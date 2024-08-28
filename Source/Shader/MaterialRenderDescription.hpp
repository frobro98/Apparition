// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Color.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Graphics/GraphicsAPIDefinitions.hpp"
#include "Shader/ShaderStages.hpp"

class ShaderResource;
struct MaterialResourceTable;

struct MaterialRenderDescription
{
	ShaderResource* shaders[ShaderStage::GfxStageCount];
	MaterialResourceTable* resources;
	ShadingModel shadingModel;
	CullingMode cullMode;
	FillMode fillMode;
	BlendMode blendMode;
};