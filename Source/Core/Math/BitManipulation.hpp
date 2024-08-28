// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

namespace Math
{
union BitManip_64
{
	f64 floatVal;
	i64 intVal;
};

union BitManip_32
{
	f32 floatVal;
	i32 intVal;
};

forceinline i32 AsInt32(f32 val)
{
	BitManip_32 u;
	u.intVal = 0;
	u.floatVal = val;

	return u.intVal;
}

forceinline u32 AsUint32(f32 val)
{
	return (u32)AsInt32(val);
}

forceinline f32 AsFloat32(i32 val)
{
	BitManip_32 u;
	u.floatVal = 0;
	u.intVal = val;

	return u.floatVal;
}

forceinline f32 AsFloat32(u32 val)
{
	return AsFloat32((i32)val);
}

forceinline i64 AsInt64(f64 val)
{
	BitManip_64 u;
	u.intVal = 0;
	u.floatVal = val;

	return u.intVal;
}

forceinline u64 AsUint64(f64 val)
{
	return (u64)AsInt64(val);
}

forceinline f64 AsFloat64(i64 val)
{
	BitManip_64 u;
	u.floatVal = 0;
	u.intVal = val;

	return u.floatVal;
}

forceinline f64 AsFloat64(u64 val)
{
	return AsFloat64((i64)val);
}


}

