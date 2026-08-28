#pragma once

#include "Utilities/TemplateUtils.hpp"

#define ENUM_CLASS_OPERATORS(EnumType)  \
constexpr EnumType operator|(EnumType lhs, EnumType rhs)                                                        \
{                                                                                                               \
    return static_cast<EnumType>(                                                                               \
        static_cast<__underlying_type(EnumType)>(lhs) | static_cast<__underlying_type(EnumType)>(rhs)           \
    );                                                                                                          \
}                                                                                                               \
                                                                                                                \
constexpr EnumType operator&(EnumType lhs, EnumType rhs)                                                        \
{                                                                                                               \
    return static_cast<EnumType>(                                                                               \
        static_cast<__underlying_type(EnumType)>(lhs) & static_cast<__underlying_type(EnumType)>(rhs)           \
    );                                                                                                          \
}                                                                                                               \
                                                                                                                \
constexpr EnumType operator^(EnumType lhs, EnumType rhs)                                                        \
{                                                                                                               \
    return static_cast<EnumType>(                                                                               \
        static_cast<__underlying_type(EnumType)>(lhs) ^ static_cast<__underlying_type(EnumType)>(rhs)           \
        );                                                                                                      \
}

template<is_enum Enum>
constexpr bool HasAnyEnumFlags(Enum e, Enum flag)
{
    return ((__underlying_type(Enum))e & (__underlying_type(Enum))flag) != 0;
}
