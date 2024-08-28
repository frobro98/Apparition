// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "CoreAPI.hpp"

// Data Structure that has hash map semantics in a linear, more memory/cache friendly structure
template <typename TypeA, typename TypeB>
class CORE_TEMPLATE ListPair
{
public:

public:
	const DynamicArray<TypeA>& first;
	const DynamicArray<TypeB>& second;

private:
	DynamicArray<TypeA> firstList;
	DynamicArray<TypeB> secondList;
};

