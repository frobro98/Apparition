#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

//  Order is from low-order byte to high-order byte of digest.
//   Each byte is printed with high-order hexadecimal digit first.
struct MD5Output
{
   u32 dWord_0;   // [0][1][2][3]      // lowest byte
   u32 dWord_1;
   u32 dWord_2;
   u32 dWord_3;   // [12][13][14][15]  // highest order byte
};

// take a raw buffer and find a hash number (16-bytes) see above)
CORE_API void MD5Buffer (u8* buffer, u32 buffLen, MD5Output &out);
