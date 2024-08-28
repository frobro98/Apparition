// Copyright 2020, Nathan Blane

#include "ModelFactory.h"
#include "Model.h"


Model* ModelFactory::CreateModel(Mesh* mesh, Material* material)
{
	Model* model = new Model(mesh, material);
	
	return model;
}

void ModelFactory::DestroyModel(Model * model)
{
	delete model;
}

ModelFactory& ModelFactory::Instance()
{
	static ModelFactory mfInstance;
	return mfInstance;
}
