// Copyright 2020, Nathan Blane

#pragma once

#ifdef SHADER_EXPORT
#define SHADER_API __declspec(dllexport)
#else
#define SHADER_API __declspec(dllimport)
#endif

#define SHADER_TEMPLATE

