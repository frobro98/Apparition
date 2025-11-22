// Copyright 2020, Nathan Blane

#pragma once

WALL_WRN_PUSH
#include <type_traits>
WALL_WRN_POP

#include "CoreAPI.hpp"

struct CORE_API Uncopyable
{
	Uncopyable() = default;
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};

static_assert(!std::is_copy_constructible_v<Uncopyable> && !std::is_copy_assignable_v<Uncopyable>, "Uncopyable somehow is able to be copied!");
