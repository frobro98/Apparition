
#include "MemoryFunctions.hpp"

namespace Memory
{
void Memfill(void* mem, u32 value, size_t memSize)
{
	u8* data = reinterpret_cast<u8*>(mem);
	size_t offset = 0;
	while (offset < memSize)
	{
		size_t diffSize = memSize - offset;

		size_t size = diffSize < sizeof(u32) ? diffSize : sizeof(u32);
		Memcpy(data + offset, &value, size);
		offset += sizeof(u32);
	};
}
}