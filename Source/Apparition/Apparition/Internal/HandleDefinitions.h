#pragma once

/*
* *********************************
* Anatomy of a Handle in Apparition
* *********************************
* 
* -- Vulkan, and therefore Apparition, has multi-device support. Each device houses all of its resources internally
* 
* -- Each resource must know of what device it belongs to. 
*   -- Some resources, like CommandPools and DescriptorPools, manage their own resources, and these resources require knowledge of
*      these structures as well.
* 
* -- The relationship between resource and the owner of the resource is uneven. For example, in a system that Apparition would likely 
*    support, it would be unlikely to have more than 2 devices at once. Because of this uneven relationship, a handle of a resource 
*    can be split to give multiple kinds of data
* 
*                      ******************************
*                      64-bit Handle Data Description
*                      ******************************
*     
*       |       1 byte       |     3 bytes     |          4 bytes          |
*      /+++++++++++++++++++++|++++++++++++++++++\                           \
*     |                      |                   |                           \
*     | device and pool data | handle generation | index into resource array  |
*/

// 2 bits of device index, which supports 4 total devices, due to 0 being invalid
#define DEVICE_INDEX_SHIFT 62ull
// 6 bits of pool index, which support 63 pools a single resource type could belong to, due to 0 being invalid
#define POOL_INDEX_SHIFT   56ull
// 24 bits of resource handle generation, which supports 16,777,215 generations of a resource, due to 0 being invalid
#define RESOURCE_GEN_SHIFT 32ull
// 32 bits of resource handles, which supprots 4,294,967,295, due to 0 being invalid
#define RESOURCE_INDEX_MASK ((1ull << RESOURCE_GEN_SHIFT) - 1ull)

