#pragma once

#include "GameObject/GameObject.h"

class Material;

class TextureSwapObject : public GameObject
{
public:
	TextureSwapObject(Material* mat1, Material* mat2, Material* mat3);

	void SwapTexture();
private:
	static constexpr const uint32 NumMaterials = 3;
	Material* materials[NumMaterials];
	uint32 materialIndex = 0;
};