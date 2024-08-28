// Copyright 2020, Nathan Blane

#pragma once

#ifdef MUSA_EXPORT
#define MUSA_API __declspec(dllexport)
#else
#define MUSA_API __declspec(dllimport)
#endif

#define MUSA_TEMPLATE
