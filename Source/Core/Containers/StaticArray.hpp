// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Debugging/Assertion.hpp"
#include "CoreAPI.hpp"

template <class Type, u32 size>
struct CORE_TEMPLATE StaticArray
{
	static_assert(size > 0, "Empty arrays aren't allowed currently!");
	using ValueType = Type;

	constexpr ValueType* GetData() noexcept;
	constexpr const ValueType* GetData() const noexcept;
	constexpr u32 Size() const;
	constexpr Type& First() const;
	constexpr Type& Last() const;

	constexpr ValueType& operator[](u32 index);
	constexpr const ValueType& operator[](u32 index) const;

	struct Iterator final
	{
		constexpr Iterator(ValueType(&arr)[size], u32 startIndex)
			: elems(arr),
			index(startIndex)
		{
		}

		constexpr Iterator& operator++()
		{
			Assert(index < size);
			++index;
			return *this;
		}

		constexpr bool operator!=(const Iterator& other)
		{
			return index != other.index;
		}

		constexpr ValueType& operator*()
		{
			return elems[index];
		}

		ValueType* const elems;
		u32 index;
	};

	struct ConstIterator final
	{
		constexpr ConstIterator(const ValueType (&arr)[size], u32 startIndex)
			: elems(arr),
			index(startIndex)
		{
		}

		constexpr ConstIterator& operator++()
		{
			Assert(index < size);
			++index;
			return *this;
		}

		constexpr bool operator!=(const ConstIterator& other)
		{
			return elems != other.elems && index != other.index;
		}

		constexpr const ValueType& operator*()
		{
			return elems[index];
		}

		const ValueType* const elems;
		u32 index;
	};

	friend Iterator begin(StaticArray& arr) { return Iterator(arr.internalData, 0); }
	friend ConstIterator begin(const StaticArray& arr) { return ConstIterator(arr.internalData, 0); }
	friend Iterator end(StaticArray& arr) { return Iterator(arr.internalData, size); }
	friend ConstIterator end(const StaticArray& arr) { return ConstIterator(arr.internalData, size); }

	ValueType internalData[size];
};

template<class Type, u32 size>
inline constexpr Type* StaticArray<Type, size>::GetData() noexcept
{
	return internalData;
}

template<class Type, u32 size>
inline constexpr const Type* StaticArray<Type, size>::GetData() const noexcept
{
    return internalData;
}

template<class Type, u32 size>
inline constexpr u32 StaticArray<Type, size>::Size() const
{
	return size;
}

template<class Type, u32 size>
inline constexpr Type& StaticArray<Type, size>::First() const
{
	return internalData[0];
}

template<class Type, u32 size>
inline constexpr Type& StaticArray<Type, size>::Last() const
{
	return internalData[size - 1];
}

template<class Type, u32 size>
inline constexpr Type& StaticArray<Type, size>::operator[](u32 index)
{
	Assert(index < size);
	return internalData[index];
}

template<class Type, u32 size>
inline constexpr const Type& StaticArray<Type, size>::operator[](u32 index) const
{
	Assert(index < size);
	return internalData[index];
}

// StaticArray deduction guide
template<typename First, typename... Rest>
StaticArray(First, Rest...) -> StaticArray<First, 1 + sizeof...(Rest)>;


