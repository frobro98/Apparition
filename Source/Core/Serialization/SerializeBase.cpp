// Copyright 2020, Nathan Blane

#include "SerializeBase.hpp"
#include "Debugging/Assertion.hpp"

void Serialize(SerializeBase & ser, tchar c)
{
	ser.SerializeData(&c, sizeof(tchar));
}

void Serialize(SerializeBase& ser, u8 u)
{
	ser.SerializeData(&u, sizeof(u8));
}

void Serialize(SerializeBase& ser, i8 i)
{
	ser.SerializeData(&i, sizeof(i8));
}

void Serialize(SerializeBase& ser, u16 u)
{
	ser.SerializeData(&u, sizeof(u16));
}

void Serialize(SerializeBase& ser, i16 i)
{
	ser.SerializeData(&i, sizeof(i16));
}

void Serialize(SerializeBase& ser, u32 u)
{
	ser.SerializeData(&u, sizeof(u32));
}

void Serialize(SerializeBase& ser, i32 i)
{
	ser.SerializeData(&i, sizeof(i32));
}

void Serialize(SerializeBase& ser, u64 u)
{
	ser.SerializeData(&u, sizeof(u64));
}

void Serialize(SerializeBase& ser, i64 i)
{
	ser.SerializeData(&i, sizeof(i64));
}

void Serialize(SerializeBase& ser, float f)
{
	ser.SerializeData(&f, sizeof(float));
}

void Serialize(SerializeBase& ser, double d)
{
	ser.SerializeData(&d, sizeof(double));
}

void Serialize(SerializeBase& ser, const tchar* c, size_t size)
{
	Assert(c != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(c, size);
}

void Serialize(SerializeBase& ser, const u8* u, size_t size)
{
	Assert(u != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(u, size);
}

void Serialize(SerializeBase& ser, const i8* i, size_t size)
{			
	Assert(i != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(i, size);
}							
							
void Serialize(SerializeBase& ser, const u16* u, size_t size)
{			
	Assert(u != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(u, size);
}							
							
void Serialize(SerializeBase& ser, const i16* i, size_t size)
{			
	Assert(i != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(i, size);
}							
							
void Serialize(SerializeBase& ser, const u32* u, size_t size)
{			
	Assert(u != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(u, size);
}							
							
void Serialize(SerializeBase& ser, const i32* i, size_t size)
{		
	Assert(i != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(i, size);
}							
							
void Serialize(SerializeBase& ser, const u64* u, size_t size)
{		
	Assert(u != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(u, size);
}							
							
void Serialize(SerializeBase& ser, const i64* i, size_t size)
{		
	Assert(i != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(i, size);
}							
							
void Serialize(SerializeBase& ser, const f32* f, size_t size)
{			
	Assert(f != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(f, size);
}							
							
void Serialize(SerializeBase& ser, const f64* d, size_t size)
{
	Assert(d != nullptr);
	ser.SerializeData(&size, sizeof(size_t));
	ser.SerializeData(d, size);
}
