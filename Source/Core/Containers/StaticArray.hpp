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

	u32 Size() const;
	Type& First() const;
	Type& Last() const;

	ValueType& operator[](u32 index);
	const ValueType& operator[](u32 index) const;

	struct Iterator final
	{
		Iterator(ValueType(&arr)[size], u32 startIndex)
			: elems(arr),
			index(startIndex)
		{
		}

		Iterator& operator++()
		{
			Assert(index < size);
			++index;
			return *this;
		}

		bool operator!=(const Iterator& other)
		{
			return elems != other.elems && index != other.index;
		}

		ValueType& operator*()
		{
			return elems[index];
		}

		ValueType* const elems;
		u32 index;
	};

	struct ConstIterator final
	{
		ConstIterator(const ValueType (&arr)[size], u32 startIndex)
			: elems(arr),
			index(startIndex)
		{
		}

		ConstIterator& operator++()
		{
			Assert(index < size);
			++index;
			return *this;
		}

		bool operator!=(const ConstIterator& other)
		{
			return elems != other.elems && index != other.index;
		}

		const ValueType& operator*()
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
inline u32 StaticArray<Type, size>::Size() const
{
	return size;
}

template<class Type, u32 size>
inline Type& StaticArray<Type, size>::First() const
{
	return internalData[0];
}

template<class Type, u32 size>
inline Type& StaticArray<Type, size>::Last() const
{
	return internalData[size - 1];
}

template<class Type, u32 size>
inline Type& StaticArray<Type, size>::operator[](u32 index)
{
	Assert(index < size);
	return internalData[index];
}

template<class Type, u32 size>
inline const Type& StaticArray<Type, size>::operator[](u32 index) const
{
	Assert(index < size);
	return internalData[index];
}
