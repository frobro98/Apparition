// Copyright 2020, Nathan Blane

#include "MathFunctions.hpp"
#include "Math/MathAPI.hpp"


namespace Math
{

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
f32 Fmod(f32 x, f32 y)
{
	constexpr double one = 1., ZeroArr[] = { 0.0, -0.0 };
	i32 n, ix, iy;
	i64 hx, hy, hz, sx, i;
	hx = AsInt64(x);
	hy = AsInt64(y);
	sx = hx & UINT64_C(0x8000000000000000);        /* sign of x */
	hx ^= sx;                                /* |x| */
	hy &= UINT64_C(0x7fffffffffffffff);        /* |y| */
/* purge off exception values */
	if (unlikely(hy == 0 || hx >= UINT64_C(0x7ff0000000000000) || hy > UINT64_C(0x7ff0000000000000)))
	{
		/* y=0,or x not finite or y is NaN */
		return (x*y) / (x*y);
	}
	if (unlikely(hx <= hy)) {
		if (hx < hy) return x;        /* |x|<|y| return x */
		return (f32)ZeroArr[(uint64_t)sx >> 63];        /* |x|=|y| return x*0*/
	}
	/* determine ix = ilogb(x) */
	if (unlikely(hx < UINT64_C(0x0010000000000000)))
	{
		/* subnormal x */
		for (ix = -1022, i = (hx << 11); i > 0; i <<= 1) ix -= 1;
	}
	else
	{
		ix = (hx >> 52) - 1023;
	}
	/* determine iy = ilogb(y) */
	if (unlikely(hy < UINT64_C(0x0010000000000000))) {        /* subnormal y */
		for (iy = -1022, i = (hy << 11); i > 0; i <<= 1) iy -= 1;
	}
	else
	{
		iy = (hy >> 52) - 1023;
	}
	/* set up hx, hy and align y to x */
	if (likely(ix >= -1022))
	{
		hx = UINT64_C(0x0010000000000000) | (UINT64_C(0x000fffffffffffff)&hx);
	}
	else {                /* subnormal x, shift x to normal */
		n = -1022 - ix;
		hx <<= n;
	}
	if (likely(iy >= -1022))
	{
		hy = UINT64_C(0x0010000000000000) | (UINT64_C(0x000fffffffffffff)&hy);
	}
	else                 /* subnormal y, shift y to normal */
	{
		n = -1022 - iy;
		hy <<= n;
	}
	/* fix point fmod */
	n = ix - iy;
	while (n--)
	{
		hz = hx - hy;
		if (hz < 0)
		{
			hx = hx + hx;
		}
		else
		{
			if (hz == 0)                /* return sign(x)*0 */
				return (f32)ZeroArr[(u64)sx >> 63];
			hx = hz + hz;
		}
	}
	hz = hx - hy;
	if (hz >= 0) { hx = hz; }

	/* convert back to floating value and restore the sign */
	if (hx == 0)                        /* return sign(x)*0 */
	{
		return (f32)ZeroArr[(u64)sx >> 63];
	}

	while (hx < UINT64_C(0x0010000000000000))         /* normalize x */
	{
		hx = hx + hx;
		iy -= 1;
	}

	if (likely(iy >= -1022))         /* normalize output */
	{
		hx = ((hx - UINT64_C(0x0010000000000000)) | ((u64)(iy + 1023) << 52));
		x = (f32)AsFloat64(hx | sx);
	}
	else                 /* subnormal output */
	{
		n = -1022 - iy;
		hx >>= n;
		x = (f32)AsFloat64(hx | sx);
		x *= one;                /* create necessary signal */
	}

	return x;                /* exact output */
}
}
