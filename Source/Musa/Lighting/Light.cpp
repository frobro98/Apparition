// Copyright 2020, Nathan Blane

#include "Light.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/UniformBuffers.h"

Light::Light()
{
	lightInformationBuffer = GetGraphicsInterface().CreateUniformBuffer(sizeof(LightProperties));
}

Light::~Light()
{
	GetGraphicsInterface().DestroyUniformBuffer(lightInformationBuffer);
}
