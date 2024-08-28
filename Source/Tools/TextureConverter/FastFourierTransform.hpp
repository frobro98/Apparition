// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"

void FastFourier(
	u32 numSamples,
	const DynamicArray<float>& realIn, 
	const DynamicArray<float>& imageIn,
	DynamicArray<float>& realOut,
	DynamicArray<float>& imageOut
);

void InverseFastFourier(
	u32 numSamples,
	const DynamicArray<float>& realIn,
	const DynamicArray<float>& imageIn,
	DynamicArray<float>& realOut,
	DynamicArray<float>& imageOut
);