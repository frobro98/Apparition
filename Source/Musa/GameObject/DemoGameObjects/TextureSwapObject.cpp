#include "TextureSwapObject.h"
#include "Model/Model.h"

TextureSwapObject::TextureSwapObject(Material* mat1, Material* mat2, Material* mat3)
{
	materials[0] = mat1;
	materials[1] = mat2;
	materials[2] = mat3;
}

void TextureSwapObject::SwapTexture()
{
	Assert(model);
	materialIndex = (materialIndex + 1) % NumMaterials;
	model->SetMaterial(materials[materialIndex]);
}
