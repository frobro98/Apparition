// Copyright 2020, Nathan Blane

#pragma once

#include "Debugging/Assertion.hpp"

// Textures
struct NativeTexture
{
};
struct NativeSampler
{
};

// Buffers
struct NativeUniformBuffer
{
	u64 size;
};

struct NativeStorageBuffer
{
	u64 size;
};

struct NativeVertexBuffer
{
	u64 size;
};

struct NativeIndexBuffer
{
	u64 size;
};

// Viewport
struct NativeViewport
{
};

// Shaders
struct NativeVertexShader
{
};

struct NativeGeometryShader
{
};

struct NativeTessEvaluationShader 
{
};

struct NativeTessControlShader 
{
};

struct NativeFragmentShader 
{
};

struct NativeComputeShader 
{
};

