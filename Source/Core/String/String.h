// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "CoreAPI.hpp"

class SerializeBase;
class DeserializeBase;

// TODO - add functions to convert numbers to strings
class CORE_API String
{
public:
	String() = default;
	String(const String& strObj) = default;
	String(String&& strObj) noexcept = default;
	String& operator=(const String& strObj) = default;
	String& operator=(String&& strObj) noexcept = default;

	explicit String(const tchar* cStr, u32 size);
	String(const tchar* cStr);

	String& operator=(const tchar* cStr);

	u32 Length() const;
	String Trim() const;
	void Replace(const tchar* toFind, const tchar* toReplace);
	String SubStr(u32 startIndex) const;
	String SubStr(u32 startIndex, u32 endIndex) const;
	// TODO - This should return some sort of interface that can be iterated over, not a Dynamic Array...
	// TODO - This should initially be a free function that gets called by this member function
	DynamicArray<String> Split(const tchar* charToSplitOn) const;
	i32 IndexOf(tchar ch) const;
	tchar CharAt(u32 index) const;
	i32 FindFirst(const tchar* str) const;
	i32 FindLast(const tchar* str) const;
	i32 FindRange(u32 startIndex, u32 endIndex, const tchar* str) const;
	i32 FindFrom(u32 index, const tchar* str) const;

	void Add(const tchar* str);
	void Add(tchar c);
	void Insert(const tchar* str, u32 index);
	void Insert(tchar c, u32 index);
	void Remove(u32 index, u32 count = 1);
	void RemoveAll(tchar c);

	bool StartsWith(const tchar* cStr) const;
	bool StartsWith(tchar ch) const;
	bool EndsWith(const tchar* cStr) const;
	bool EndsWith(tchar ch) const;
	bool IsEmpty() const;
	bool Contains(const tchar* str) const;

	String GetUpperCase() const;
	void ToUpperCase();
	String GetLowerCase() const;
	void ToLowerCase();

	i32 Compare(const String& str) const;
	i32 Compare(const tchar* str) const;
	i32 Compare(const String& str, u32 numToCompare) const;
	i32 Compare(const tchar* str, u32 numToCompare) const;

public:
	tchar operator[](u32 index) const;
	const tchar* operator*() const;

	String& operator+=(const String& strObj);
	String& operator+=(const tchar* strObj);
	String& operator+=(tchar c);

	friend CORE_API String operator+(const String& str0, const String& str1);
	friend CORE_API String operator+(const String& str0, const tchar* str1);
	friend CORE_API String operator+(const String& str, tchar c);
	friend CORE_API String operator+(const tchar* str0, const String& str1);

private:
	DynamicArray<tchar> stringData;

public:
	// Boolean operators
	friend CORE_API bool operator==(const String& left, const String& right);
	friend CORE_API bool operator!=(const String& left, const String& right);
	friend CORE_API bool operator>(const String& left, const String& right);
	friend CORE_API bool operator<(const String& left, const String& right);
	friend CORE_API bool operator>=(const String& left, const String& right);
	friend CORE_API bool operator<=(const String& left, const String& right);

	// C-String comparisons
	friend CORE_API bool operator==(const String& left, const tchar* right);
	friend CORE_API bool operator!=(const String& left, const tchar* right);
	friend CORE_API bool operator>(const String& left, const tchar* right);
	friend CORE_API bool operator<(const String& left, const tchar* right);
	friend CORE_API bool operator>=(const String& left, const tchar* right);
	friend CORE_API bool operator<=(const String& left, const tchar* right);

	friend CORE_API bool operator==(const tchar* left, const String& right);
	friend CORE_API bool operator!=(const tchar* left, const String& right);
	friend CORE_API bool operator>(const tchar* left, const String& right);
	friend CORE_API bool operator<(const tchar* left, const String& right);
	friend CORE_API bool operator>=(const tchar* left, const String& right);
	friend CORE_API bool operator<=(const tchar* left, const String& right);

	friend CORE_API u32 GetHash(const String& str);
	friend CORE_API void Serialize(SerializeBase& ser, const String& str);
	friend CORE_API void Deserialize(DeserializeBase& ser, String& str);
};
