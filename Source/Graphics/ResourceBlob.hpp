// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Graphics/GraphicsAPI.hpp"

class SerializeBase;
class DeserializeBase;

// TODO - Currently, this object represents both general data, for resources such as textures, and for a collection of data, like index and vertex data. It might be good to separate this behavior
class GRAPHICS_API ResourceBlob final
{
public:
	ResourceBlob() = default;
	ResourceBlob(u8* blobData, size_t blobSize);
	ResourceBlob(const ResourceBlob& other);
	ResourceBlob(ResourceBlob&& other) noexcept;
	~ResourceBlob();

	ResourceBlob& operator=(const ResourceBlob& other);
	ResourceBlob& operator=(ResourceBlob&& other) noexcept;

	void MergeWith(const ResourceBlob& other);

	inline u8* GetData() { return data; }
	inline const u8* GetData() const { return data; }
	inline size_t GetSize() const { return size; }

	friend GRAPHICS_API ResourceBlob CombineBlobs(const ResourceBlob& blob0, const ResourceBlob& blob1);
	friend GRAPHICS_API void Serialize(SerializeBase& ser, const ResourceBlob& blob);
	friend GRAPHICS_API void Deserialize(DeserializeBase& ser, ResourceBlob& blob);

private:
	u8* data = nullptr;
	size_t size = 0;
};
