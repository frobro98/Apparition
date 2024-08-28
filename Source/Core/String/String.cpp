// Copyright 2020, Nathan Blane

#include "String/String.h"
#include "Utilities/HashFuncs.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "CStringUtilities.hpp"
#include "Debugging/Assertion.hpp"

// TODO - Fix string and dynamic array to use size_t/uint64

String::String(const tchar* cStr)
{
	Assert(cStr);
	size_t len = Strlen(cStr);
	if (len > 0)
	{
		stringData.Clear();
		stringData.AddDefault((u32)len + 1);
		Memcpy(stringData.GetData(), len * sizeof(tchar), cStr, len * sizeof(tchar));
	}
}

String& String::operator=(const tchar* cStr)
{
	Assert(cStr);
	size_t len = Strlen(cStr);
	if (len > 0)
	{
		stringData.Clear();
		stringData.AddDefault((u32)len + 1);
		Memcpy(stringData.GetData(), len * sizeof(tchar), cStr, len * sizeof(tchar));
	} 

	return *this;
}

String::String(const tchar* cStr, u32 size)
{
	Assert(cStr);
	if (size > 0)
	{
		u32 adjustedSize = size + 1;
		stringData.AddDefault(adjustedSize);
		Memcpy(stringData.GetData(), size * sizeof(tchar), cStr, size * sizeof(tchar));
	}
}

u32 String::Length() const
{
	return !IsEmpty() ? stringData.Size() - 1 : 0;
}

String String::Trim() const
{
	u32 numWhitespace = 0;
	const tchar* beginning = stringData.GetData();
	const tchar* ending = beginning + stringData.Size();
	Assert(*ending == '\0');
	--ending;
	while (*beginning == ' ')
	{
		++numWhitespace;
		++beginning;
	}

	while (*ending == ' ')
	{
		++numWhitespace;
		--ending;
	}


	return String(beginning, stringData.Size() - numWhitespace);
}

void String::Replace(const tchar* toFind, const tchar* toReplace)
{
	Assert(toFind);
	Assert(toReplace);

	if (!IsEmpty() && *toFind)
	{
		size_t findLen = Strlen(toFind);
		size_t replaceLen = Strlen(toReplace);

		if (findLen == replaceLen)
		{
			tchar* str = Strstr(stringData.GetData(), toFind);
			while (str != nullptr)
			{
				for (size_t i = 0; i < replaceLen; ++i)
				{
					str[i] = toReplace[i];
				}

				str = Strstr(str, toFind);
			}
		}
		else
		{
			String me(*this);
			stringData.Clear();

			tchar* original = me.stringData.GetData();
			tchar* str = Strstr(original, toFind);
			while (str != nullptr)
			{
				*str = '\0';
				(*this) += original;
				(*this) += toReplace;
				original = str + 1;
				str = Strstr(original, toFind);
			}
			(*this) += original;
		}
	}
}

String String::SubStr(u32 startIndex) const
{
	Assert(startIndex < stringData.Size());
	return String(stringData.GetData() + startIndex, stringData.Size() - startIndex);
}

String String::SubStr(u32 startIndex, u32 endIndex) const
{
	Assert(startIndex < stringData.Size());
	Assert(endIndex < stringData.Size() + 1);
	u32 diff = startIndex + (stringData.Size() - endIndex);
	return String(stringData.GetData() + startIndex, stringData.Size() - diff);
}

DynamicArray<String> String::Split(const tchar* charToSplitOn) const
{
	// TODO - Implement string split
	UNUSED(charToSplitOn);
	return DynamicArray<String>();
}

i32 String::IndexOf(tchar ch) const
{
	i32 indexOf = 0;
	while (stringData[indexOf] != '\0')
	{
		if (stringData[indexOf] == ch)
		{
			return indexOf;
		}
		++indexOf;
	}

	return -1;
}

tchar String::CharAt(u32 index) const
{
	Assert(index < stringData.Size());
	return stringData[index];
}

i32 String::FindFirst(const tchar* str) const
{
	return FindFirstIn(stringData.GetData(), Length(), str, Strlen(str));
}

i32 String::FindLast(const tchar* str) const
{
	return FindLastIn(stringData.GetData(), Length(), str, Strlen(str));
}

i32 String::FindRange(u32 startIndex, u32 endIndex, const tchar* str) const
{
	Assert(str);
	Assert(startIndex < stringData.Size());
	Assert(startIndex < endIndex);
	Assert(endIndex <= stringData.Size());
	size_t searchStrLen = Strlen(str);
	i32 foundFirstIndex = FindFirstIn(stringData.GetData() + startIndex, endIndex - startIndex, str, searchStrLen);
	return foundFirstIndex + startIndex;
}

i32 String::FindFrom(u32 index, const tchar* str) const
{
	Assert(str);
	Assert(index < Length());
	size_t searchStrLen = Strlen(str);
	i32 foundFirstIndex = FindFirstIn(stringData.GetData() + index, Length() - index, str, searchStrLen);
	return foundFirstIndex + index;
}

void String::Add(const tchar* str)
{
	u32 oldLen = Length();
	size_t cStrLen = Strlen(str);
	stringData.Resize(oldLen + (u32)cStrLen + 1);
	Strcat(stringData.GetData(), stringData.Size(), str, cStrLen);
}

void String::Add(tchar c)
{
	const u32 oldLen = Length();
	constexpr u32 cStrLen = 1;
	stringData.Resize(oldLen + cStrLen + 1);
	stringData[oldLen + 1] = c;
	stringData.Last() = 0;
}

// void String::Insert(const tchar* str, uint32 index)
// {
//
void String::Insert(tchar c, u32 index)
{
	stringData.Insert(c, index);
}

void String::Remove(u32 index, u32 count)
{
	Assert(index + count <= Length());
	stringData.Remove(index, count);
}

void String::RemoveAll(tchar c)
{
	stringData.RemoveAll(c);
}

bool String::StartsWith(const tchar* cStr) const
{
	return ::StartsWith(stringData.GetData(), Length(), cStr, Strlen(cStr));
}

bool String::StartsWith(tchar ch) const
{
	return stringData[0] == ch;
}

bool String::EndsWith(const tchar* cStr) const
{
	return ::EndsWith(stringData.GetData(), Length(), cStr, Strlen(cStr));
}

bool String::EndsWith(tchar ch) const
{
	// account for null terminator
	return stringData[stringData.Size() - 2] == ch;
}

bool String::IsEmpty() const
{
	return stringData.Size() == 0;
}

bool String::Contains(const tchar* str) const
{
	return Strstr(stringData.GetData(), str);
}

String String::GetUpperCase() const
{
	String upper(*this);
	upper.ToUpperCase();
	return upper;
}

void String::ToUpperCase()
{
	for (u32 i = 0; i < Length(); ++i)
	{
		stringData[i] = ToUpper(stringData[i]);
	}
}

String String::GetLowerCase() const
{
	String lower(*this);
	lower.ToLowerCase();
	return lower;
}

void String::ToLowerCase()
{
	for (u32 i = 0; i < Length(); ++i)
	{
		stringData[i] = ToLower(stringData[i]);
	}
}

i32 String::Compare(const String& str) const
{
	return Strcmp(stringData.GetData(), str.stringData.GetData());
}

i32 String::Compare(const tchar* str) const
{
	return Strcmp(stringData.GetData(), str);
}

i32 String::Compare(const String& str, u32 numToCompare) const
{
	return Strncmp(**this, *str, numToCompare);
}

i32 String::Compare(const tchar* str, u32 numToCompare) const
{
	return Strncmp(**this, str, numToCompare);
}

tchar String::operator[](u32 index) const
{
	Assert(index < stringData.Size());
	return stringData[index];
}

String& String::operator+=(const String& strObj)
{
	u32 oldLen = stringData.Size();
	stringData.Resize(oldLen + strObj.Length());
	Strcat(stringData.GetData(), stringData.Size() + 1, strObj.stringData.GetData(), strObj.stringData.Size());

	return *this;
}

String& String::operator+=(const tchar* strObj)
{
	const u32 oldLen = Length();
	const size_t cStrLen = Strlen(strObj);
	stringData.Resize(oldLen + (u32)cStrLen + 1);
	Strcat(stringData.GetData(), stringData.Size(), strObj, cStrLen);

	return *this;
}

String& String::operator+=(tchar c)
{
	const u32 oldLen = Length();
	constexpr u32 cStrLen = 1;
	stringData.Resize(oldLen + cStrLen + 1);
	stringData[oldLen] = c;
	stringData.Last() = 0;
	return *this;
}

const tchar* String::operator*() const
{
	return stringData.GetData();
}

String operator+(const String& str0, const String& str1)
{
	String s(str0);
	s += str1;
	return s;
}

String operator+(const String& str0, const tchar* str1)
{
	String s(str0);
	s += str1;
	return s;
}

String operator+(const tchar* str0, const String& str1)
{
	String s(str0);
	s += str1;
	return s;
}

bool operator==(const String& left, const String& right)
{
	return left.Compare(right) == 0;
}

bool operator!=(const String& left, const String& right)
{
	return left.Compare(right) != 0;
}

bool operator>(const String& left, const String& right)
{
	return left.Compare(right) > 0;
}

bool operator<(const String& left, const String& right)
{
	return left.Compare(right) < 0;
}

bool operator>=(const String& left, const String& right)
{
	return left.Compare(right) >= 0;
}

bool operator<=(const String& left, const String& right)
{
	return left.Compare(right) <= 0;
}

bool operator==(const String& left, const tchar* right)
{
	return left.Compare(right) == 0;
}

bool operator!=(const String& left, const tchar* right)
{
	return left.Compare(right) != 0;
}

bool operator>(const String& left, const tchar* right)
{
	return left.Compare(right) > 0;
}

bool operator<(const String& left, const tchar* right)
{
	return left.Compare(right) < 0;
}

bool operator>=(const String& left, const tchar* right)
{
	return left.Compare(right) >= 0;
}

bool operator<=(const String& left, const tchar* right)
{
	return left.Compare(right) <= 0;
}

bool operator==(const tchar* left, const String& right)
{
	return Strcmp(left, right.stringData.GetData()) == 0;
}

bool operator!=(const tchar* left, const String& right)
{
	return Strcmp(left, right.stringData.GetData()) != 0;
}

bool operator>(const tchar* left, const String& right)
{
	return Strcmp(left, right.stringData.GetData()) > 0;
}

bool operator<(const tchar* left, const String& right)
{
	return Strcmp(left, right.stringData.GetData()) < 0;
}

bool operator>=(const tchar* left, const String& right)
{
	return Strcmp(left, right.stringData.GetData()) >= 0;
}

bool operator<=(const tchar* left, const String& right)
{
	return Strcmp(left, right.stringData.GetData()) <= 0;
}

//////////////////////////////////////////////////////////////////////////
// String hash definition
//////////////////////////////////////////////////////////////////////////

u32 GetHash(const String& str)
{
	return fnv32(str.stringData.GetData(), static_cast<u32>(str.stringData.Size()));
}

void Serialize(SerializeBase& ser, const String& str)
{
	Serialize(ser, str.Length());
	for (u32 i = 0; i < str.Length(); ++i)
	{
		Serialize(ser, str.stringData[i]);
	}
}

void Deserialize(DeserializeBase& ser, String& str)
{
	u32 stringLen;
	Deserialize(ser, stringLen);

	str.stringData.Resize(stringLen + 1);
	for (u32 i = 0; i < stringLen; ++i)
	{
		Deserialize(ser, str.stringData[i]);
	}
}
