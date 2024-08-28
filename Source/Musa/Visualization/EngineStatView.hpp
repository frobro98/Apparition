// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"

struct ScreenTextItem;

class EngineStatView
{
public:

	void ToggleStats();
	bool AreStatsVisible() const;

	void PushStatsToView(DynamicArray<ScreenTextItem>& textItems);

private:
	bool statsVisible = false;
};

