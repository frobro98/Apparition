// Copyright 2020, Nathan Blane

#include "ShaderResource.hpp"
#include "Graphics/GraphicsInterface.hpp"

ShaderResource::ShaderResource(const ShaderHeader& header_, const MemoryBuffer& codeBuffer)
	: header(header_),
	code(codeBuffer)
{
	if (header.stage == ShaderStage::Vertex)
	{
		vertexShader = GetGraphicsInterface().CreateVertexShader(codeBuffer);
	}
	else if (header.stage == ShaderStage::Fragment)
	{
		fragmentShader = GetGraphicsInterface().CreateFragmentShader(codeBuffer);
	}
	else
	{
		AssertStr(false, "Shader stage passed in currently isn't supported!");
	} 
}

ShaderResource::~ShaderResource()
{
	if (header.stage == ShaderStage::Vertex)
	{
		GetGraphicsInterface().DestroyVertexShader(vertexShader);
	}
	else if (header.stage == ShaderStage::Fragment)
	{
		GetGraphicsInterface().DestroyFragmentShader(fragmentShader);
	}
	else
	{
		AssertStr(false, "Shader stage passed in currently isn't supported!");
	}
}
