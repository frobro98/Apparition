
#pragma once

#ifdef MATH_EXPORT
#define MATH_API __declspec(dllexport)
#else
#define MATH_API __declspec(dllimport)
#endif

