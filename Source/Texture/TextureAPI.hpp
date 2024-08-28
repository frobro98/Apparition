// Copyright 2020, Nathan Blane

#pragma once

#ifdef TEX_EXPORT
#define TEX_API __declspec(dllexport)
#else
#define TEX_API __declspec(dllimport)
#endif

//#define TEX_TEMPLATE

