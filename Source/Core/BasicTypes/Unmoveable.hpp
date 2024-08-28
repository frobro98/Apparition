// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

struct Unmoveable
{
	Unmoveable() = default;
	Unmoveable(Unmoveable&&) = delete;
	Unmoveable& operator=(Unmoveable&&) = delete;
};

static_assert(!std::is_move_constructible_v<Unmoveable> && !std::is_move_assignable_v<Unmoveable>, "Uncopyable somehow is able to be copied!");
