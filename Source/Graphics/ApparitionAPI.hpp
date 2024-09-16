// Copyright 2020, Nathan Blane

#pragma once

#ifdef APPARITION_EXPORT
#define APPARITION_API __declspec(dllexport)
#else
#define APPARITION_API __declspec(dllimport)
#endif

