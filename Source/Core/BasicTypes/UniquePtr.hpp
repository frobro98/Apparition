// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

#include "CoreFlags.hpp"
#include "BasicTypes/Utility.hpp"

template <typename OwnedType>
class UniquePtr
{
	static_assert(!std::is_array_v<OwnedType>, "Cannot use dynamic arrays in unique ptr");
public:
	UniquePtr() = default;
	UniquePtr(OwnedType* owningPtr);
	template <typename Type, typename = std::enable_if_t<std::is_convertible_v<Type*, OwnedType*>>>
	UniquePtr(UniquePtr<Type>&& up) noexcept
		: ptr(up.Release())
	{
	}

	~UniquePtr();

	UniquePtr(UniquePtr&&) noexcept;
	UniquePtr& operator=(UniquePtr&&) noexcept;
	template <typename Type, typename = std::enable_if_t<std::is_convertible_v<Type*, OwnedType*>>>
	UniquePtr& operator=(UniquePtr<Type>&& other) noexcept
	{
		Reset(other.Release());
		return *this;
	}

	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;

	OwnedType& operator*() const;
	OwnedType* operator->() const;

	bool IsValid() const;
	operator bool() const;

	NODISCARD OwnedType* Get() const;
	NODISCARD OwnedType* Release() noexcept;
	void Reset(OwnedType* newPtr = nullptr);

	void Swap(UniquePtr& other);

private:
	void ReleaseOwnedPtr();

private:
	OwnedType* ptr = nullptr;
};

template <typename Type>
inline void Swap(UniquePtr<Type>& lhs, UniquePtr<Type>& rhs)
{
	lhs.Swap(rhs);
}

template<typename OwnedType, typename... Args>
inline UniquePtr<OwnedType> MakeUnique(Args&&... args)
{
	return UniquePtr<OwnedType>(new OwnedType(FORWARD(Args, args)...));
}

template<typename OwnedType>
inline UniquePtr<OwnedType>::UniquePtr(OwnedType* owningPtr)
	: ptr(owningPtr)
{
}

template<typename OwnedType>
inline UniquePtr<OwnedType>::~UniquePtr()
{
	ReleaseOwnedPtr();
}

template<typename OwnedType>
inline UniquePtr<OwnedType>::UniquePtr(UniquePtr&& other) noexcept
{
	ptr = other.ptr;
	other.ptr = nullptr;
}

template<typename OwnedType>
inline UniquePtr<OwnedType>& UniquePtr<OwnedType>::operator=(UniquePtr&& other) noexcept
{
	if (this != &other)
	{
		ReleaseOwnedPtr();
		ptr = other.ptr;
		other.ptr = nullptr;
	}

	return *this;
}

template<typename OwnedType>
inline OwnedType& UniquePtr<OwnedType>::operator*() const
{
	return *ptr;
}

template<typename OwnedType>
inline OwnedType* UniquePtr<OwnedType>::operator->() const
{
	return ptr;
}

template<typename OwnedType>
inline bool UniquePtr<OwnedType>::IsValid() const
{
	return ptr != nullptr;
}

template<typename OwnedType>
UniquePtr<OwnedType>::operator bool() const
{
	return ptr != nullptr;
}

template<typename OwnedType>
inline OwnedType* UniquePtr<OwnedType>::Get() const
{
	return ptr;
}

template<typename OwnedType>
inline void UniquePtr<OwnedType>::Reset(OwnedType* newPtr)
{
	ReleaseOwnedPtr();
	ptr = newPtr;
}

template<typename OwnedType>
inline void UniquePtr<OwnedType>::Swap(UniquePtr& other)
{
	OwnedType* tmp = ptr;
	ptr = other.ptr;
	other.ptr = tmp;
}

template<typename OwnedType>
inline void UniquePtr<OwnedType>::ReleaseOwnedPtr()
{
	delete ptr;
	ptr = nullptr;
}

template<typename OwnedType>
inline OwnedType* UniquePtr<OwnedType>::Release() noexcept
{
	OwnedType* p = ptr;
	ptr = nullptr;
	return p;
}

