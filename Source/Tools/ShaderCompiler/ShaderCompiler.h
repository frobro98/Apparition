// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct ShaderCompilerDefinitions;
struct ShaderStructure;
struct PreprocessedShaderOutput;

void InitializeCompiler();
void DeinitializeCompiler();
bool Preprocess(const tchar* pathToShader, const ShaderCompilerDefinitions& inputs, PreprocessedShaderOutput& output);
bool Compile(const tchar* pathToShader, const char* entryPoint, const ShaderCompilerDefinitions& inputs, ShaderStructure& outputs);