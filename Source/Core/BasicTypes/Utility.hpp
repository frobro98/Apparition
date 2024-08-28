

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/MacroHelpers.hpp"

//////////////////////////////////////////////////////////////////////////
// declval
//////////////////////////////////////////////////////////////////////////
namespace Internal
{
template<typename T> T&& declval_(long);
template<typename T> T declval_(int);
}

template<typename T>
decltype(Internal::declval_<T>(0)) declval() noexcept;

//////////////////////////////////////////////////////////////////////////
// integral_constant
//////////////////////////////////////////////////////////////////////////
template<typename T, T val>
struct integral_constant
{
	static constexpr T value = val;

	using value_type = T;
	using type = integral_constant;

	constexpr operator value_type() const noexcept
	{
		return value;
	}

	NODISCARD constexpr value_type operator()() const noexcept
	{
		return value;
	}
};

//////////////////////////////////////////////////////////////////////////
// bool_constant && true_type/false_type
//////////////////////////////////////////////////////////////////////////
template<bool B>
using bool_constant = integral_constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

//////////////////////////////////////////////////////////////////////////
// void_t
// is_void
//////////////////////////////////////////////////////////////////////////

/* void_t */
template<typename... Args>
using void_t = void;

/* is_void */
template<typename T>
struct is_void
	: false_type
{
};

#define CLASS_DEFINE_CV(func) \
	func(EMPTY_ARGUMENT) \
	func(const) \
	func(volatile) \
	func(const volatile)

#define IS_VOID(cv_opt) \
	template<> \
	struct is_void<cv_opt void> \
		: true_type \
	{ \
	};

CLASS_DEFINE_CV(IS_VOID)
#undef IS_VOID
#undef CLASS_DEFINE_CV

template<typename T>
inline constexpr bool is_void_v = is_void<T>::value;

//////////////////////////////////////////////////////////////////////////
// alignment_of
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct alignment_of
	: integral_constant<size_t, alignof(T)>
{
};

//////////////////////////////////////////////////////////////////////////
// add_const
// add_lvalue_reference
// add_rvalue_reference
//////////////////////////////////////////////////////////////////////////

/* add_const */
template<typename T>
struct add_const
{
	using type = const T;
};

template<typename T>
using add_const_t = typename add_const<T>::type;

/* add_l/rvalue_reference */
namespace Internal
{
template<typename T, typename = void>
struct AddRef
{
	using lval = T;
	using rval = T;
};

template<typename T>
struct AddRef<T, void_t<T&>>
{
	using lval = T&;
	using rval = T&&;
};
}

template<typename T>
struct add_lvalue_reference
{
	using type = typename Internal::AddRef<T>::lval;
};

template<typename T>
struct add_rvalue_reference
{
	using type = typename Internal::AddRef<T>::rval;
};

template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

//////////////////////////////////////////////////////////////////////////
// remove_const
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct remove_const
{
	using type = T;
};

template<typename T>
struct remove_const<const T>
{
	using type = T;
};

template <typename T>
using remove_const_t = typename remove_const<T>::type;

//////////////////////////////////////////////////////////////////////////
// remove_volatile
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct remove_volatile
{
	using type = T;
};

template<typename T>
struct remove_volatile<volatile T>
{
	using type = T;
};

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

//////////////////////////////////////////////////////////////////////////
// remove_cv
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct remove_cv
{
	using type = T;
};

template<typename T>
struct remove_cv<const T>
{
	using type = T;
};

template<typename T>
struct remove_cv<volatile T>
{
	using type = T;
};

template<typename T>
struct remove_cv<const volatile T>
{
	using type = T;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

//////////////////////////////////////////////////////////////////////////
// remove_reference
//////////////////////////////////////////////////////////////////////////
template<typename T>
struct remove_reference
{
	using type = T;
};

template<typename T>
struct remove_reference<T&>
{
	using type = T;
};

template<typename T>
struct remove_reference<T&&>
{
	using type = T;
};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

//////////////////////////////////////////////////////////////////////////
// is_reference
// is_pointer
//////////////////////////////////////////////////////////////////////////

/* is_reference */
template<typename T>
struct is_reference
	: false_type
{
};

template<typename T>
struct is_reference<T&>
	: true_type
{
};

template<typename T>
struct is_reference<T&&>
	: true_type
{
};

template<typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

/* is_pointer */
template<typename T>
struct is_pointer
	: false_type
{
};

template<typename T>
struct is_pointer<T*>
	: true_type
{
};

template<typename T>
struct is_pointer<T* const>
	: true_type
{
};

template<typename T>
struct is_pointer<T* volatile>
	: true_type
{
};

template<typename T>
struct is_pointer<T* const volatile>
	: true_type
{
};

template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

//////////////////////////////////////////////////////////////////////////
// is_convertible
//////////////////////////////////////////////////////////////////////////
namespace Internal
{
template<class T>
auto test_returnable(int) -> decltype(
	void(static_cast<T(*)()>(nullptr)), true_type{}
);
template<class>
auto test_returnable(...)-> false_type;

template<class From, class To>
auto test_implicitly_convertible(int) -> decltype(
	void(declval<void(&)(To)>()(declval<From>())), true_type{}
);
template<class, class>
auto test_implicitly_convertible(...)-> false_type;

} // namespace detail

template<class From, class To>
struct is_convertible : integral_constant<bool,
	(decltype(Internal::test_returnable<To>(0))::value&&
		decltype(Internal::test_implicitly_convertible<From, To>(0))::value) ||
	(is_void_v<From>&& is_void_v<To>)
> 
{
};

//////////////////////////////////////////////////////////////////////////
// is_constructible
// is_nothrow_constructible
// is_nothrow_copy_constructible
// is_nothrow_move_constructible
//////////////////////////////////////////////////////////////////////////

/* is_constructible */
namespace Internal
{
template <typename, class T, typename... Args>
struct is_constructible_ : false_type
{
};

template <class T, typename... Args>
struct is_constructible_<
	void_t<decltype(::new T(declval<Args>()...))>, T, Args...>
	: true_type
{
};
}

template <class T, typename... Args>
using is_constructible = Internal::is_constructible_<void_t<>, T, Args...>;

template <class T, typename... Args>
inline constexpr bool is_constructible_v = is_constructible<T, Args...>

/* is_nothrow_constructible */
// namespace Interenal
// {
// template<typename T>
// void InterenalConversionTo(T) noexcept {}
// 
// template<bool, bool, class T, typename... Args>
// struct internal_is_nothrow_constructible;
// 
// template<class T, typename... Args>
// struct internal_is_nothrow_constructible</*is_constructible*/true, /*is_reference*/false, T, Args...>
// 	: integral_constant<bool, noexcept(T(declval<Args>()...))
// {
// };
// 
// template<class T, typename Arg>
// struct internal_is_nothrow_constructible</*is_constructible*/true, /*is_reference*/true, T, Arg>
// 	: integral_constant<bool, noexcept(InterenalConversionTo<T>(declval<Arg>()))>
// {
// };
// 
// template<class T, bool IsRef, typename... Args>
// struct internal_is_nothrow_constructible</*is_constructible*/false, IsRef, T, Args...>
// 	: false_type
// {
// };
// }
// 
// template<class T, typename... Args>
// struct is_nothrow_constructible
// 	: Internal::internal_is_nothrow_constructible<is_constructible_v<T, Args...>, is_reference_v<T>, T, Args...>
// {
// };

// template<class T, u64 N>
// struct is_nothrow_constructible<T[N]>
// 	: Internal::internal_is_nothrow_constructible<is_constructible_v<T>, is_reference_v<T>, T>
// {
// };

template<class T, typename... Args>
struct is_nothrow_constructible;

template<class T, typename... Args>
struct is_nothrow_constructible
	: integral_constant<bool, __is_nothrow_constructible(T, Args...)>
{
};

template<class T, typename... Args>
inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

/* is_nothrow_copy_constructible */
template<typename T>
struct is_nothrow_copy_constructible
	: is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>>
{
};

template<typename T>
inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;

/* is_nothrow_move_constructible */
template<typename T>
struct is_nothrow_move_constructible
	: is_nothrow_constructible<T, add_rvalue_reference_t<T>>
{
};

template<typename T>
inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

//////////////////////////////////////////////////////////////////////////
// enable_if
//////////////////////////////////////////////////////////////////////////
template<bool B, typename T = void>
struct enable_if
{
};

template<typename T>
struct enable_if<true, T>
{
	using type = T;
};

template<bool B, typename T>
using enable_if_t = typename enable_if<B, T>::type;

//////////////////////////////////////////////////////////////////////////
// conditional
//////////////////////////////////////////////////////////////////////////
template<bool Test,
	typename T0, typename T1>
	struct conditional
{
	using type = T1;
};

template<typename T0, typename T1>
struct conditional<true, T0, T1>
{
	using type = T0;
};

template<bool Test,
	typename T0, typename T1>
	using conditional_t = typename conditional<Test, T0, T1>::type;


#define MOVE(value) static_cast<remove_reference_t<decltype(value)> &&>(value)
#define FORWARD(type, value) static_cast<type &&>(value)
