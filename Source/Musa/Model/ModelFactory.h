// Copyright 2020, Nathan Blane

#pragma once

class Model;
class Mesh;
class Material;
class RenderContext;

class ModelFactory
{
public:
	static Model* CreateModel(Mesh* mesh, Material* material);
	static void DestroyModel(Model* model);

private:
	ModelFactory() = default;
	~ModelFactory() = default;

	static ModelFactory& Instance();

private:
	RenderContext* renderer;

};