

#pragma once

#include "BasicTypes/Intrinsics.hpp"

namespace Musa
{
enum class ApplicationEventType
{
	Invalid,
	Resize,
	Close,
	Activated,
	Deactivated
};

struct ResizeData
{
	i32 width{ 0 };
	i32 height{ 0 };
};

struct ApplicationEvent
{
	ResizeData resizeData{};
	ApplicationEventType type{ ApplicationEventType::Invalid };
};
}
