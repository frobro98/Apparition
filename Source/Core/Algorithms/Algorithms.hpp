// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"

// TODO - Move this to somewhere that makes more sense than this...
template<typename T = void>
struct Less
{
	constexpr bool operator()(const T& lhs, const T& rhs) const
	{
		return lhs < rhs;
	}
};

template<typename T = void>
struct Greater
{
	constexpr bool operator()(const T& lhs, const T& rhs) const
	{
		return lhs > rhs;
	}
};


template <typename Exch>
void Swap(Exch& e0, Exch& e1)
{
	Exch tmp = e0;
	e0 = e1;
	e1 = tmp;
}

// TODO - Combine the 2 different implementations for insertion sort
template<class Elem>
void InsertionSort(DynamicArray<Elem>& arr)
{
	i32 size = static_cast<i32>(arr.Size());
	for (i32 i = 1; i < size; ++i)
	{
		for (i32 j = i - 1; j >= 0; --j)
		{
			u32 index = static_cast<u32>(j);
			if (arr[index + 1] < arr[index])
			{
				Swap(arr[index], arr[index + 1]);
			}
		}
	}

	Assert(IsSorted(arr.GetData(), arr.Size(), Less<Elem>{}));
}

template <typename T, size_t N>
void InsertionSort(T(&arr)[N])
{
	for (size_t i = 1; i < N; ++i)
	{
		i64 index = (i64)i;
		for (i64 j = index - 1; j >= 0; --j)
		{
			if (arr[j + 1] < arr[j])
			{
				Swap(arr[j], arr[j + 1]);
			}
		}
	}

	Assert(IsSorted(arr, N, Less<T>{}));
}

template <typename T>
constexpr void InsertionSort(T* arr, size_t count)
{
	for (size_t i = 1; i < count; ++i)
	{
		i64 index = (i64)i;
		for (i64 j = index - 1; j >= 0; --j)
		{
			if (arr[j + 1] < arr[j])
			{
				Swap(arr[j], arr[j + 1]);
			}
		}
	}

	Assert(IsSorted(arr, count, Less<T>{}));
}

template <typename T, typename Pred>
constexpr bool IsSorted(const T* arr, size_t count, Pred&& func)
{
	for (u32 i = 1; i < count; ++i)
	{
		if (!func(arr[i - 1], arr[i]))
		{
			return false;
		}
	}

	return true;
}

// template<class Elem, typename Pred>
// void Partition(DynamicArray<Elem>& arr, Pred&& partFunc)
// {
// 	for (uint32 i = 0; i < arr.Size(); ++i)
// 	{
// 		if()
// 	}
// }


