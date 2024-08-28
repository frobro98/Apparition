// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

class CORE_API DeserializeBase
{
public:
	DeserializeBase() = default;
	virtual ~DeserializeBase() = default;

	virtual void DeserializeData(void* data, size_t dataSize) = 0;

	friend CORE_API void Deserialize(DeserializeBase& ser, tchar& c);
	friend CORE_API void Deserialize(DeserializeBase& ser, u8& uint8);
	friend CORE_API void Deserialize(DeserializeBase& ser, i8& int8);
	friend CORE_API void Deserialize(DeserializeBase& ser, u16& uint16);
	friend CORE_API void Deserialize(DeserializeBase& ser, i16& int16);
	friend CORE_API void Deserialize(DeserializeBase& ser, u32& uint32);
	friend CORE_API void Deserialize(DeserializeBase& ser, i32& int32);
	friend CORE_API void Deserialize(DeserializeBase& ser, u64& uint64);
	friend CORE_API void Deserialize(DeserializeBase& ser, i64& int64);
	friend CORE_API void Deserialize(DeserializeBase& ser, f32& f);
	friend CORE_API void Deserialize(DeserializeBase& ser, f64& d);

	friend CORE_API void Deserialize(DeserializeBase& ser, tchar*& c, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, u8*& uint16, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, i8*& int6, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, u16*& uint16, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, i16*& int16, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, u32*& uint32, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, i32*& int32, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, u64*& uint64, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, i64*& int64, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, f32*& f, size_t& size);
	friend CORE_API void Deserialize(DeserializeBase& ser, f64*& d, size_t& size);
};