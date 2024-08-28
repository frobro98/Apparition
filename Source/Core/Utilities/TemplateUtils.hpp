// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

#include "CoreFlags.hpp"

template <typename Type, typename... Types>
struct all_same_type
{
	static constexpr bool IsAllSame()
	{
		bool result = true;
		NOT_USED int dummy[] = { 0, (result = result && std::is_same_v<Type, Types>, 0)... };
		return result;
	}
	//static constexpr bool value = (std::is_same_v<Type, Types> && ...);
	static constexpr bool value = IsAllSame();
};

template <typename Type, typename... Types>
inline constexpr bool all_same_type_v = all_same_type<Type, Types...>::value;

template <typename Base, typename... Derived>
struct all_base_of
{
	static constexpr bool IsAllBase()
	{
		bool result = true;
		NOT_USED int dummy[] = { 0, (result = result && std::is_base_of_v<Base, Derived>, 0)... };
		return result;
	}
	static constexpr bool value = IsAllBase();
};

template <typename Base, typename... Derived>
inline constexpr bool all_base_of_v = all_base_of<Base, Derived...>::value;

template<typename ConvertTo, typename... ConvertFrom>
struct all_convertable_to
{
	static constexpr bool AreAllConvertable()
	{
		bool result = true;
		NOT_USED int dummy[] = { 0, (result = result && std::is_convertible_v<ConvertFrom, ConvertTo>, 0)... };
		return result;
	}

	static constexpr bool value = AreAllConvertable();
};

template<typename ConvertTo, typename... ConvertFrom>
inline constexpr bool all_convertable_to_v = all_convertable_to<ConvertTo, ConvertFrom...>::value;

template <typename Src, typename Dst>
struct is_memcpy_constructable
{
	static_assert(std::conjunction_v<all_same_type_v<Src, Dst>>, "Can't memcpy an object into a different typed object");
	static_assert(!std::is_reference_v<Src> && !std::is_reference_v<Dst>, "References can't be used with memcpy");

	static constexpr bool value = false;
};

template <typename T>
struct is_memcpy_constructable<T, T>
{
	static constexpr bool value = std::is_trivially_copy_constructible_v<T>;
};

template <typename T>
struct is_memcpy_constructable<const T*, T*>
{
	static constexpr bool value = true;
};

template <typename Src, typename Dst>
struct is_memcpy_constructable<const Src, Dst> : is_memcpy_constructable<Src, Dst>
{
};

template <typename Src, typename Dst>
inline constexpr bool is_memcpy_constructable_v = is_memcpy_constructable<Src, Dst>::value;

// This is for static_assert calls that need to be always false, but can't be given false because it just wouldn't compile
template<typename...>
struct always_false
{
	static constexpr bool value = false;
};

template <typename... Args>
inline constexpr bool always_false_v = always_false<Args...>::value;

