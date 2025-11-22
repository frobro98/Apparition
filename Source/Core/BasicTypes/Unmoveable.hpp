// Copyright 2020, Nathan Blane

#pragma once

WALL_WRN_PUSH
#include <type_traits>
WALL_WRN_POP

struct Unmoveable
{
	Unmoveable() = default;
	Unmoveable(Unmoveable&&) = delete;
	Unmoveable& operator=(Unmoveable&&) = delete;
};

static_assert(!std::is_move_constructible_v<Unmoveable> && !std::is_move_assignable_v<Unmoveable>, "Uncopyable somehow is able to be copied!");
