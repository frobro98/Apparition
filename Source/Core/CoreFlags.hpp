// Copyright 2020, Nathan Blane

#pragma once

#define M_DEBUG _DEBUG
#define M_RELEASE !M_DEBUG

#define forceinline __forceinline

#define likely(x) (x)
#define unlikely(x) (x)

#define UNUSED(...)						\
	__pragma(warning(push))				\
	__pragma(warning(disable : 4548))	\
	(__VA_ARGS__)						\
	__pragma(warning(pop))

#define NOT_USED [[maybe_unused]]
#define NODISCARD [[nodiscard]]

#define WALL_WRN_PUSH				  \
	__pragma(warning( push ))		  \
	__pragma(warning(disable : 4005)) \
	__pragma(warning(disable : 4061)) \
	__pragma(warning(disable : 4100)) \
	__pragma(warning(disable : 4127)) \
	__pragma(warning(disable : 4189)) \
	__pragma(warning(disable : 4267)) \
	__pragma(warning(disable : 4324)) \
	__pragma(warning(disable : 4365)) \
	__pragma(warning(disable : 4464)) \
	__pragma(warning(disable : 4571)) \
	__pragma(warning(disable : 4623)) \
	__pragma(warning(disable : 4625)) \
	__pragma(warning(disable : 4626)) \
    __pragma(warning(disable : 4774)) \
	__pragma(warning(disable : 4820)) \
    __pragma(warning(disable : 5026)) \
    __pragma(warning(disable : 5027)) \
	__pragma(warning(disable : 5039)) \
	__pragma(warning(disable: 26451)) \
	__pragma(warning(disable: 26495))

#define WALL_WRN_POP \
	__pragma(warning( pop ))

