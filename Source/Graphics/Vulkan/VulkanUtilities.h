// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAbstractions.h"
#include "Shader/ShaderStages.hpp"
#include "Texture/ImageFormats.h"
#include "Graphics/VertexInputDescription.hpp"
#include "Graphics/GraphicsAPIDefinitions.hpp"
#include "Graphics/GraphicsAPI.hpp"
// TODO - Remove this shit
#include "Shader/ShaderStructure.hpp"

class VulkanDevice;
class VulkanCommandBuffer;
struct VulkanStagingBuffer;

VkAccessFlags GetAccessFlagsFor(VkImageLayout layout);
VkPipelineStageFlags GetStageFor(VkImageLayout layout);

bool PresentationSupported(VkPhysicalDevice gpu, u32 queueIndex);

void ImageLayoutTransition(VulkanCommandBuffer& cmdBuffer, const VkImageSubresourceRange& resourceRange, VkImageLayout newLayouts, const DynamicArray<VulkanImage*>& images);
void CopyToDeviceBuffer(VulkanCommandBuffer& cmdBuffer, const VulkanBuffer& stagingBuffer, const VulkanBuffer& dstBuffer);
void CopyStagingBufferToImage(VulkanCommandBuffer& cmdBuffer, const VulkanStagingBuffer& buffer, const VulkanImage& image);
void CopyBufferToImage(VulkanCommandBuffer& cmdBuffer, const VulkanBuffer& buffer, const VulkanImage& image);
void CopyImage(VulkanCommandBuffer& cmdBuffer, const VulkanImage& srcImage, const VulkanImage& dstImage);

// TODO - Move these into a vulkan format conversion file or something
// TODO - Have these not be exported because it's unnecessary
GRAPHICS_API VkDescriptorType MusaConstantToDescriptorType(ShaderConstant constant);
VkFormat MusaFormatToVkFormat(ImageFormat format);
VkImageAspectFlags MusaFormatToVkAspect(ImageFormat format);
VkFormat MusaInputsToVk(VertexInputType type);
VkPrimitiveTopology MusaTopologyToVk(PrimitiveTopology topology);
VkPolygonMode MusaPolyToVk(FillMode mode);
VkCullModeFlags MusaCullToVk(CullingMode mode);
VkCompareOp MusaCompareOpToVk(CompareOperation op);
VkAttachmentLoadOp MusaLoadToVk(LoadOperation op);
VkAttachmentStoreOp MusaStoreToVk(StoreOperation op);
VkBlendOp MusaBlendOpToVk(BlendOperation op);
VkBlendFactor MusaBlendFactorToVk(BlendFactor factor);
GRAPHICS_API VkShaderStageFlagBits MusaStageToVkStage(ShaderStage::Type shaderStage);
VkFilter MusaFilterToVkFilter(SamplerFilter filter);
VkSamplerAddressMode MusaAddressModeToVkAddressMode(SamplerAddressMode addrMode);
VkSamplerMipmapMode MusaMipModeToVkMipMode(SamplerMipmapMode mipMode);

