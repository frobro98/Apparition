#pragma once

#include "BasicTypes/Intrinsics.hpp"

template <typename T, u32 N>
constexpr size_t ArraySize(NOT_USED const T(&arr)[N])
{
	return N;
}

