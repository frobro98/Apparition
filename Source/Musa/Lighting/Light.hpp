// Copyright 2020, Nathan Blane

#pragma once

//#include "GameObject/GameObject.h"
#include "BasicTypes/UniquePtr.hpp"
#include "Math/Matrix4.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "BasicTypes/Color.hpp"

class VulkanDevice;

struct LightDescription
{
	Vector4 position;
	Color color;
	Vector4 direction;
	float cosOuterAngle = 0; // Because shader is doing dot product using this information, the outer angle is kept in cosine space, allowing the shader to do angle calculations
	float inverseCosDiff = 0; // 1 / (innerCos - outerCos)
	float radius = 0;
};

class Light //: public GameObject
{
public:
	Light();
	virtual ~Light();

	virtual LightDescription GetLightDescription() = 0;

	inline Vector4 GetDirection() const
	{
		return Vector4(-worldLight[m2], -worldLight[m6], -worldLight[m10]);
	}
	inline const Matrix4& GetWorldLightTransform() const
	{
		return worldLight;
	}

	inline NativeUniformBuffer& GetLightBuffer() { return *lightInformationBuffer; }

	Color lightColor;

protected:
	Matrix4 worldLight;
	NativeUniformBuffer* lightInformationBuffer = nullptr;
};