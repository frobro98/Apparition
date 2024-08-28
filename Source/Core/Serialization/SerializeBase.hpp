// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

class CORE_API SerializeBase
{
public:
	SerializeBase() = default;
	virtual ~SerializeBase() = default;

	virtual void SerializeData(const void* data, size_t dataSize) = 0;

	friend CORE_API void Serialize(SerializeBase& ser, tchar c);
	friend CORE_API void Serialize(SerializeBase& ser, u8 u);
	friend CORE_API void Serialize(SerializeBase& ser, i8 i);
	friend CORE_API void Serialize(SerializeBase& ser, u16 u);
	friend CORE_API void Serialize(SerializeBase& ser, i16 i);
	friend CORE_API void Serialize(SerializeBase& ser, u32 u);
	friend CORE_API void Serialize(SerializeBase& ser, i32 i);
	friend CORE_API void Serialize(SerializeBase& ser, u64 u);
	friend CORE_API void Serialize(SerializeBase& ser, i64 i);
	friend CORE_API void Serialize(SerializeBase& ser, f32 f);
	friend CORE_API void Serialize(SerializeBase& ser, f64 d);

	// Raw pointer data serialization
	friend CORE_API void Serialize(SerializeBase& ser, const tchar* c, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const u8* u, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const i8* i, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const u16* u, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const i16* i, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const u32* u, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const i32* i, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const u64* u, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const i64* i, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const f32* f, size_t size);
	friend CORE_API void Serialize(SerializeBase& ser, const f64* d, size_t size);
};
