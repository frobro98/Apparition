// Copyright 2020, Nathan Blane

#include "MaterialPrimitiveBatches.hpp"
#include "Containers/DynamicArray.hpp"
#include "Graphics/GraphicsInterface.hpp"

void MaterialPrimitiveBatches::AddMaterialBatch(const MaterialPrimitiveBatch& primBatch)
{
	batches.Add(primBatch);
}
