// Copyright 2020, Nathan Blane

#pragma once

#include "Utilities/HashBasicTypes.h"
#include "Containers/DynamicArray.hpp"
#include "Containers/Pair.h"
#include "CoreAPI.hpp"

template<typename Key, typename Value>
class CORE_TEMPLATE Map
{
	using KeyType = Key;
	using ValueType = Value;
	using MapType = Pair<KeyType, ValueType>;
	using BucketType = DynamicArray<MapType>;
public:
	Map()
		: buckets(DefaultAmountOfBuckets)
	{
	}

	explicit Map(u32 bucketAmount)
		: buckets(bucketAmount)
	{
	}

	Map(std::initializer_list<MapType> list)
		: buckets(DefaultAmountOfBuckets)
	{
		const MapType* beginning = list.begin();
		const MapType* ending = list.end();
		for (; beginning != ending; ++beginning)
		{
			Add(beginning->first, beginning->second);
		}
	}

	Map(const Map& other)
		: buckets(other.buckets),
		size(other.size),
		maxBucketSize(other.maxBucketSize)
	{
	}

	Map(Map&& other)
		: buckets(MOVE(other.buckets)),
		size(other.size),
		maxBucketSize(other.maxBucketSize)
	{
		other.maxBucketSize = 0;
	}

	Map& operator=(const Map& other)
	{
		if (this != &other)
		{
			buckets = other.buckets;
			size = other.size;
			maxBucketSize = other.maxBucketSize;
		}

		return *this;
	}

	Map& operator=(Map&& other)
	{
		if (this != &other)
		{
			buckets = MOVE(other.buckets);
			size = other.size;
			maxBucketSize = other.maxBucketSize;

			other.maxBucketSize = 0;
		}

		return *this;
	}

public:
	ValueType& Add(const KeyType& key, const ValueType& val)
	{
		u32 hash = GetHash(key);
		BucketType& bucket = FindBucket(hash);
		MapType* store = FindPair(bucket, key);
		if (store)
		{
			store->second = val;
		}
		else
		{
			Pair<KeyType, ValueType> pair = { key, val };
			u32 index = bucket.Add(pair);
			store = &bucket[index];
			++size;
		}

		return store->second;
	}

	ValueType* Find(const KeyType& key)
	{
		ValueType* value = nullptr;
		u32 hash = GetHash(key);
		BucketType& bucket = FindBucket(hash);
		MapType* store = FindPair(bucket, key);
		if (store)
		{
			value = &store->second;
		}

		return value;
	}

	const ValueType* Find(const KeyType& key) const
	{
		// TODO - This is awful and really shouldn't be like this. Find a way without a const_cast
		return const_cast<Map*>(this)->Find(key);
	}

	bool TryFind(const KeyType& key, ValueType& value)
	{
		u32 hash = GetHash(key);
		BucketType& bucket = FindBucket(hash);
		MapType* store = FindPair(bucket, key);
		if (store)
		{
			value = store->second;
			return true;
		}

		return false;
	}

	bool Remove(const KeyType& key)
	{
		u32 hash = GetHash(key);
		BucketType& bucket = FindBucket(hash);
		return RemoveBucketElement(bucket, key);
	}

	inline u32 Size() const
	{
		return size;
	}

	inline bool IsEmpty() const
	{
		return size == 0;
	}

private:
	bool RemoveBucketElement(BucketType& bucket, const KeyType& key)
	{
		for (u32 i = 0; i < buckets.Size(); ++i)
		{
			if (bucket[i].first == key)
			{
				bucket.Remove(i);
				--size;
				return true;
			}
		}
		return false;
	}

	BucketType& FindBucket(u32 hash)
	{
		u32 bucketIndex = hash % buckets.Size();
		return buckets[bucketIndex];
	}

	MapType* FindPair(BucketType& bucket, const KeyType& key)
	{
		MapType* storage = nullptr;
		for (u32 i = 0; i < bucket.Size(); ++i)
		{
			MapType& store = bucket[i];
			if (store.first == key)
			{
				storage = &store;
				break;
			}
		}
		return storage;
	}

	template <typename EmplaceType>
	ValueType& FindOrDefault(EmplaceType&& key)
	{
		u32 hash = GetHash(key);
		BucketType& bucket = FindBucket(hash);
		MapType* storage = FindPair(bucket, key);

		if (!storage)
		{
			Pair<KeyType, ValueType> pair = { FORWARD(EmplaceType, key), ValueType() };
			u32 index = bucket.Add(pair);
			storage = &bucket[index];
		}

		return storage->second;
	}

public:
	ValueType& operator[](const KeyType& key)
	{
		return FindOrDefault(key);
	}

	ValueType& operator[](KeyType&& key)
	{
		return FindOrDefault(MOVE(key));
	}

public:
	class Iterator final
	{
		friend class Map<Key, Value>;

	public:
		Iterator(Map<Key, Value>& map)
			: buckets(&map.buckets)
		{
			u32 i = bucketIndex;
			for (; i < buckets->Size(); ++i)
			{
				if ((*buckets)[i].Size() > 0)
				{
					break;
				}
			}
			bucketIndex = i;
		}

		Iterator(Map<Key, Value>& map, u32 bucketInd)
			: buckets(&map.buckets),
			bucketIndex(bucketInd)
		{
		}

		MapType& operator*()
		{
			return (*buckets)[bucketIndex][bucketElementIndex];
		}

		explicit operator bool() const
		{
			return bucketIndex < buckets->Size();
		}

		Iterator& operator++()
		{
			if (++bucketElementIndex >= (*buckets)[bucketIndex].Size())
			{
				bucketElementIndex = 0;
				u32 i = bucketIndex + 1;
				for (; i < buckets->Size(); ++i)
				{
					if ((*buckets)[i].Size() > 0)
					{
						break;
					}
				}

				bucketIndex = i;
			}

			return *this;
		}

		friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
		{
			return lhs.buckets != rhs.buckets || 
				lhs.bucketIndex != rhs.bucketIndex || 
				lhs.bucketElementIndex != rhs.bucketElementIndex;
		}

	private:
		DynamicArray<BucketType>* buckets;
		u32 bucketIndex = 0;
		u32 bucketElementIndex = 0;

	};

	class ConstIterator final
	{
		friend class Map<Key, Value>;

	public:
		ConstIterator(const Map<Key, Value>& map)
			: buckets(&map.buckets)
		{
			u32 i = bucketIndex;
			for (; i < buckets->Size(); ++i)
			{
				if ((*buckets)[i].Size() > 0)
				{
					break;
				}
			}
			bucketIndex = i;
		}

		ConstIterator(const Map<Key, Value>& map, u32 bucketInd)
			: buckets(&map.buckets),
			bucketIndex(bucketInd)
		{
		}

		const MapType& operator*()
		{
			return (*buckets)[bucketIndex][bucketElementIndex];
		}

		explicit operator bool() const
		{
			return bucketIndex < buckets->Size();
		}

		ConstIterator& operator++()
		{
			if (++bucketElementIndex >= (*buckets)[bucketIndex].Size())
			{
				bucketElementIndex = 0;
				u32 i = bucketIndex + 1;
				for (; i < buckets->Size(); ++i)
				{
					if ((*buckets)[i].Size() > 0)
					{
						break;
					}
				}

				bucketIndex = i;
			}

			return *this;
		}

		friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs)
		{
			return lhs.buckets != rhs.buckets ||
				lhs.bucketIndex != rhs.bucketIndex ||
				lhs.bucketElementIndex != rhs.bucketElementIndex;
		}

	private:
		const DynamicArray<BucketType>* buckets;
		u32 bucketIndex = 0;
		u32 bucketElementIndex = 0;

	};

private:
	friend Iterator begin(Map& map) { return Iterator(map); }
	friend ConstIterator begin(const Map& map) { return ConstIterator(map); }
	friend Iterator end(Map& map) { return Iterator(map, map.buckets.Size()); }
	friend ConstIterator end(const Map& map) { return ConstIterator(map, map.buckets.Size()); }

	friend void Serialize(SerializeBase& ser, const Map& map)
	{
		Serialize(ser, map.size);
		Serialize(ser, map.maxBucketSize);
		Serialize(ser, map.buckets);
	}

	friend void Deserialize(DeserializeBase& ser, Map& map)
	{
		Deserialize(ser, map.size);
		Deserialize(ser, map.maxBucketSize);
		Deserialize(ser, map.buckets);
	}

private:
	static constexpr u32 DefaultAmountOfBuckets = 32;
	DynamicArray<BucketType> buckets;
	u32 size = 0;
	u32 maxBucketSize = 0;
};
