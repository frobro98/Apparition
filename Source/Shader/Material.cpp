// Copyright 2020, Nathan Blane

#include "Material.hpp"
#include "Texture2D/TextureManager.h"
#include "Texture2D/Texture.h"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/UniformBuffers.h"
#include "Shader/ShaderResourceManager.hpp"

Material::Material(ShaderID vertexID, ShaderID fragmentID)
{
	ShaderResource* vertShader = GetShaderResourceManager().FindShaderResource(vertexID);
	Assert(vertShader != nullptr);
	ShaderResource* fragShader = GetShaderResourceManager().FindShaderResource(fragmentID);
	Assert(fragShader != nullptr);

	shader.Initialize(*vertShader, *fragShader);
	materialProperties.diffuse = Color32::White();

	renderDescription = MakeUnique<MaterialRenderDescription>();
	renderDescription->shaders[ShaderStage::Vertex] = vertShader;
	renderDescription->shaders[ShaderStage::Fragment] = fragShader;
	renderDescription->resources = &shader.GetMaterialResourceTable();

	materialPropsBuffer = GetGraphicsInterface().CreateUniformBuffer(sizeof(MaterialProperties));
	materialPropsConstant = GetUniformBufferConstant(MaterialPropertiesParameterName);
}

MaterialRenderDescription& Material::GetRenderDescription()
{
	if (renderDescDirty)
	{
		renderDescDirty = false;

		GetGraphicsInterface().PushBufferData(*materialPropsBuffer, &materialProperties);
		SetUniformBufferResource(materialPropsConstant, *materialPropsBuffer);

		renderDescription->blendMode = blendMode;
		renderDescription->cullMode = cullMode;
		renderDescription->fillMode = fillMode;
		renderDescription->shadingModel = shadingModel;
	}

	return *renderDescription;
}

