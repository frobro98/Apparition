// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Color.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "Math/Matrix4.hpp"
#include "Math/Vector4.hpp"

constexpr u32 MaxBones = 120;

constexpr const tchar* PrimitiveParameterName = "prim";
constexpr const tchar* ViewParameterName = "view";
constexpr const tchar* MaterialPropertiesParameterName = "materialProperties";

struct PrimUniformBuffer
{
	Matrix4 model;
};

struct ViewUniformBuffer
{
	Matrix4 viewTransform;
	Matrix4 projectionTransform;
	Vector4 viewPosition;
};

struct MaterialProperties
{
	Color32 diffuse;
	// 	f32 roughness = 0.f;
	// 	f32 metallic = 0.f;
};

struct PoseInverseUniformBuffer
{
	Matrix4 inverses[MaxBones];

	PoseInverseUniformBuffer()
	{
		Memset(inverses, 0, sizeof(Matrix4) * MaxBones);
	}
};

struct BoneDeltasUniformBuffer
{
	Matrix4 deltaTransforms[MaxBones];

	BoneDeltasUniformBuffer()
	{
		Memset(deltaTransforms, 0, sizeof(Matrix4) * MaxBones);
	}
};

struct LightProperties
{
	Vector4 position;
	Vector4 direction;
	Color32 color;
	Matrix4 lightViewTransform;
	Matrix4 lightProjection;//lightSpaceTransform;
};

struct KeyframeConstanceUniformBuffer
{
	u32 boneCount;
	float deltaTime;
};

struct BoneHierarchyTable
{
	struct BoneTableElement
	{
		static constexpr u32 MaxElementLength = 20;
		u32 boneParentHierarchy[MaxElementLength];
	};

	BoneTableElement table[MaxBones];
};