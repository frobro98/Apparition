// Copyright 2020, Nathan Blane

#pragma once

#ifdef GRAPHICS_EXPORT
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

