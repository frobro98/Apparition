// Copyright 2020, Nathan Blane

#pragma once

#include <iterator>

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"
WALL_WRN_PUSH
#include "fmt/format.h"
WALL_WRN_POP

namespace Debug
{
CORE_API void Print(const tchar* str);
CORE_API void Print(const fmt::memory_buffer& buf);

template <typename... Args>
CORE_TEMPLATE void Printf(const tchar* strFormat, Args... args)
{
	fmt::memory_buffer buf;
	fmt::vformat_to(std::back_inserter(buf), strFormat, fmt::make_format_args(args...));
	Print(buf);
}
}