#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct Window
{
	void* windowHandle = nullptr;
	i32 posX = 0, posY = 0;
	i32 width = 0, height = 0;
};

Window* CreateSandboxWindow(void* instance, i32 xPos, i32 yPos, i32 width, i32 height);