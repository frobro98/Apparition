// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Debugging/Assertion.hpp"
#include "Containers/DynamicArray.hpp"
#include "CoreAPI.hpp"

// TODO - Need to support size_t instead of uint32 
template <typename ViewType>
class CORE_TEMPLATE ArrayView
{
	using pointer_type = ViewType*;
	using reference_type = ViewType&;
	using value_type = ViewType;

	template <typename OtherType>
	friend class ArrayView;
public:
	constexpr ArrayView(ViewType* dataToView, u32 dataSize);
	template <u32 N>
	constexpr ArrayView(pointer_type (&arr)[N]);
	ArrayView(DynamicArray<value_type>& arr);

	template <typename OtherType>
	constexpr ArrayView(const ArrayView<OtherType>& other) noexcept;
	constexpr ArrayView(const ArrayView&) noexcept = default;

	constexpr ArrayView& operator=(const ArrayView&) noexcept = default;

	constexpr ViewType& First() const;
	constexpr ViewType& Last() const;

	constexpr ViewType* GetData();
	constexpr u32 Size() const;
	constexpr u32 SizeInBytes() const;
	constexpr bool IsEmpty() const;

	ViewType& operator[](u32 index);
	const ViewType& operator[](u32 index) const;

private:
	friend pointer_type begin(ArrayView& arr) { return arr.viewData; }
	friend pointer_type begin(const ArrayView& arr) { return arr.viewData; }
	friend pointer_type end(ArrayView& arr) { return arr.viewData + arr.viewSize; }
	friend pointer_type end(const ArrayView& arr) { return arr.viewData + arr.viewSize; }

private:
	pointer_type viewData;
	u32 viewSize;
};

template<typename ViewType>
inline constexpr ArrayView<ViewType>::ArrayView(ViewType* dataToView, u32 dataSize)
	: viewData(dataToView), viewSize(dataSize)
{
}

template<typename ViewType>
inline ArrayView<ViewType>::ArrayView(DynamicArray<value_type>& arr)
	: viewData(arr.GetData()), viewSize(arr.Size())
{
}


template<typename ViewType>
template<u32 N>
inline constexpr ArrayView<ViewType>::ArrayView(pointer_type(&arr)[N])
	: viewData(arr), viewSize(N)
{
}

template<typename ViewType>
template <typename OtherType>
inline constexpr ArrayView<ViewType>::ArrayView(const ArrayView<OtherType>& other) noexcept
	: viewData(other.viewData), viewSize(other.viewSize)
{
}

template<typename ViewType>
inline constexpr ViewType& ArrayView<ViewType>::First() const
{
	Assert(viewData != nullptr);
	Assert(viewSize != 0);
	return viewData[0];
}

template<typename ViewType>
inline constexpr ViewType& ArrayView<ViewType>::Last() const
{
	Assert(viewData != nullptr);
	Assert(viewSize != 0);
	return viewData[viewSize - 1];
}

template<typename ViewType>
inline constexpr ViewType* ArrayView<ViewType>::GetData()
{
	return viewData;
}

template<typename ViewType>
inline constexpr u32 ArrayView<ViewType>::Size() const
{
	return viewSize;
}

template<typename ViewType>
inline constexpr u32 ArrayView<ViewType>::SizeInBytes() const
{
	return viewSize * sizeof(ViewType);
}

template<typename ViewType>
inline constexpr bool ArrayView<ViewType>::IsEmpty() const
{
	return viewSize == 0;
}

template<typename ViewType>
inline ViewType& ArrayView<ViewType>::operator[](u32 index)
{
	Assert(viewData != nullptr);
	Assert(viewSize != 0);
	Assert(index < viewSize);

	return viewData[index];
}

template<typename ViewType>
inline const ViewType& ArrayView<ViewType>::operator[](u32 index) const
{
	Assert(viewData != nullptr);
	Assert(viewSize != 0);
	Assert(index < viewSize);

	return viewData[index];
}

