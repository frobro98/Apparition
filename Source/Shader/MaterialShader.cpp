// Copyright 2020, Nathan Blane

#include "MaterialShader.hpp"
#include "ShaderResource.hpp"

void MaterialShader::Initialize(ShaderResource& vertexShader, ShaderResource& fragmentShader)
{
	shaderResources[ShaderStage::Vertex] = &vertexShader;
	shaderResources[ShaderStage::Fragment] = &fragmentShader;
	ConstructResourceTable();
}

u16 MaterialShader::GetUniformBufferResourceIndex(const tchar* ubName) const
{
	for (const auto& desc : resourceTable.resourceBindings)
	{
		if (desc.type == ShaderResourceType::UniformBuffer &&
			desc.name == ubName)
		{
			return desc.resourceIndex;
		}
	}

	return 0xffff;
}

u16 MaterialShader::GetTextureSamplerResourceIndex(const tchar* texName) const
{
	for (const auto& desc : resourceTable.resourceBindings)
	{
		if (desc.type == ShaderResourceType::TextureSampler &&
			desc.name == texName)
		{
			return desc.resourceIndex;
		}
	}

	return 0xffff;
}

void MaterialShader::SetUniformBufferResource(u16 resourceIndex, const NativeUniformBuffer& uniformBuffer)
{
	Assert(resourceIndex  < resourceTable.uniformBufferStorage.Size());
	resourceTable.uniformBufferStorage[resourceIndex] = &uniformBuffer;
}

void MaterialShader::SetTextureSamplerResource(u16 resourceIndex, const NativeTexture& texture, const NativeSampler& sampler)
{
	Assert(resourceIndex < resourceTable.textureStorage.Size());
	resourceTable.textureStorage[resourceIndex] = &texture;
	resourceTable.samplerStorage[resourceIndex] = &sampler;
}

void MaterialShader::ConstructResourceTable()
{
	ShaderResource* vertexShaderResource = shaderResources[ShaderStage::Vertex];
	const ShaderConstantTable& vertexTable = vertexShaderResource->GetResourceTable();
	ShaderResource* fragmentShaderResource = shaderResources[ShaderStage::Fragment];
	const ShaderConstantTable& fragmentTable = fragmentShaderResource->GetResourceTable();

	const u32 totalUBs = vertexTable.uniformBufferResourceInfo.Size() + fragmentTable.uniformBufferResourceInfo.Size();
	const u32 totalTexs = vertexTable.textureResourceInfo.Size() + fragmentTable.textureResourceInfo.Size();
	const u32 totalBindings = totalTexs + totalUBs;
	
	resourceTable.resourceBindings.Resize(totalBindings);
	resourceTable.uniformBufferStorage.Reserve(totalUBs);
	resourceTable.textureStorage.Reserve(totalTexs);
	resourceTable.samplerStorage.Reserve(totalTexs);

	for (const auto& vsUB : vertexTable.uniformBufferResourceInfo)
	{
		Assert(resourceTable.resourceBindings[vsUB.bindIndex].usage == 0);
		
		MaterialResourceDesc desc;
		desc.name = vsUB.name;
		desc.type = ShaderResourceType::UniformBuffer;
		desc.bindIndex = vsUB.bindIndex;
		desc.size = vsUB.size;
		desc.usage = VertexMask;
		desc.resourceIndex = (u16)resourceTable.uniformBufferStorage.Size();
		resourceTable.resourceBindings[vsUB.bindIndex] = desc;

		resourceTable.uniformBufferStorage.AddDefault();
	}

	for (const auto& vsTex : vertexTable.textureResourceInfo)
	{
		Assert(resourceTable.resourceBindings[vsTex.bindIndex].usage == 0);

		MaterialResourceDesc desc;
		desc.name = vsTex.name;
		desc.type = ShaderResourceType::TextureSampler;
		desc.bindIndex = vsTex.bindIndex;
		desc.size = vsTex.size;
		desc.usage = VertexMask;
		desc.resourceIndex = (u16)resourceTable.textureStorage.Size();
		resourceTable.resourceBindings[vsTex.bindIndex] = desc;

		resourceTable.textureStorage.AddDefault();
		resourceTable.samplerStorage.AddDefault();
	}

	for (const auto& fsUB : fragmentTable.uniformBufferResourceInfo)
	{
		const u16 bindIndex = fsUB.bindIndex;
		if (auto& matDesc = resourceTable.resourceBindings[bindIndex]; matDesc.usage > 0)
		{
			Assert(matDesc.type == ShaderResourceType::UniformBuffer);
			matDesc.usage |= FragmentMask;
			resourceTable.resourceBindings.RemoveLast();
		}
		else
		{
			MaterialResourceDesc desc;
			desc.name = fsUB.name;
			desc.type = ShaderResourceType::UniformBuffer;
			desc.bindIndex = fsUB.bindIndex;
			desc.size = fsUB.size;
			desc.usage = FragmentMask;
			desc.resourceIndex = (u16)resourceTable.uniformBufferStorage.Size();
			resourceTable.resourceBindings[bindIndex] = desc;

			resourceTable.uniformBufferStorage.AddDefault();
		}
	}

	for (const auto& fsTex : fragmentTable.textureResourceInfo)
	{
		const u16 bindIndex = fsTex.bindIndex;
		if (auto& matDesc = resourceTable.resourceBindings[bindIndex]; matDesc.usage > 0)
		{
			Assert(matDesc.type == ShaderResourceType::TextureSampler);
			matDesc.usage |= FragmentMask;
			resourceTable.resourceBindings.RemoveLast();
		}
		else
		{
			MaterialResourceDesc desc;
			desc.name = fsTex.name;
			desc.type = ShaderResourceType::TextureSampler;
			desc.bindIndex = fsTex.bindIndex;
			desc.size = fsTex.size;
			desc.usage = FragmentMask;
			desc.resourceIndex = (u16)resourceTable.textureStorage.Size();
			resourceTable.resourceBindings[bindIndex] = desc;

			resourceTable.textureStorage.AddDefault();
			resourceTable.samplerStorage.AddDefault();
		}
	}

	Assert(resourceTable.textureStorage.Size() == resourceTable.samplerStorage.Size());
	Assert(resourceTable.textureStorage.Size() + resourceTable.uniformBufferStorage.Size() == 
		resourceTable.resourceBindings.Size());
	resourceTable.resourceBindings.ShrinkToFit();
}
