// Copyright 2020, Nathan Blane

#pragma once

#include <utility>

#define counter_read_crumb( TAG, RANK, ACC ) counter_crumb( TAG(), constant_index< RANK >(), constant_index< ACC >() )
#define read_counter( TAG ) counter_read_crumb( TAG, 1, counter_read_crumb( TAG, 2, counter_read_crumb( TAG, 4, counter_read_crumb( TAG, 8, \
    counter_read_crumb( TAG, 16, counter_read_crumb( TAG, 32, counter_read_crumb( TAG, 64, counter_read_crumb( TAG, 128, 0 ) ) ) ) ) ) ) )

#define increment_counter( TAG ) \
constant_index< read_counter( TAG ) + 1 > \
constexpr counter_crumb( TAG, constant_index< ( read_counter( TAG ) + 1 ) & ~ read_counter( TAG ) >, \
          					constant_index< ( read_counter( TAG ) + 1 ) & read_counter( TAG ) > ) { return {}; }

//#define COUNTER_LINK_NAMESPACE( NS ) using NS::counter_crumb;

template< std::size_t n >
struct constant_index : std::integral_constant< std::size_t, n > {};

template< typename id, std::size_t rank, std::size_t acc >
constexpr constant_index< acc > counter_crumb(id, constant_index< rank >, constant_index< acc >) { return {}; } // found by ADL via constant_index
