// Copyright 2020, Nathan Blane

#pragma once

#include "Graphics/RenderContext.hpp"
#include "Vulkan/VulkanRenderState.hpp"

struct GraphicsPipelineDescription;
class VulkanVertexBuffer;
struct VulkanFrameTempAllocation;

class VulkanRenderContext : public RenderContext
{
public:
	VulkanRenderContext(VulkanDevice& device);

	virtual void BeginRenderFrame(NativeViewport& viewport) override;
	virtual void EndRenderFrame(NativeViewport& renderViewport) override;

	virtual void InitializeWithRenderState(GraphicsPipelineDescription& pipelineDesc) const override;

	virtual void BeginRenderPass(const BeginRenderPassInfo& beginInfo) override;
	virtual void EndRenderPass() override;

	virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, float minDepth, float maxDepth) override;
	virtual void SetScissor(u32 offsetX, u32 offsetY, u32 width, u32 height) override;
	virtual void SetVertexBuffer(const NativeVertexBuffer& vertexBuffer) override;
	virtual void SetGraphicsPipeline(const GraphicsPipelineDescription& pipelineDesc) override;
	virtual void SetGlobalUniform(const void* uniformData, u32 dataSize) override;
	virtual void SetUniformBuffer(const NativeUniformBuffer& uniformBuffer, u32 bufferIndex) override;
	virtual void SetTexture(const NativeTexture& texture, const NativeSampler& sampler, u32 textureIndex) override;

	virtual void DrawPrimitive(u32 vertexCount, u32 instanceCount) override;
	virtual void DrawPrimitiveIndexed(const NativeIndexBuffer& indexBuffer, u32 numPrimitives, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance) override;
	virtual void DrawRaw(const ResourceArray& rawVerts, u32 instanceCount) override;
	virtual void DrawRawIndexed(const ResourceArray& rawVerts, const ResourceArray& rawIndices, u32 indexStride, u32 instanceCount) override;

	virtual NativeTexture* GetBackBuffer() const override;

	virtual void TransitionToWriteState(const NativeTexture** textures, u32 textureCount) override;
	virtual void TransitionToReadState(const NativeTexture** textures, u32 textureCount) override;


private:
	void UpdateViewState(VulkanCommandBuffer& cmdBuffer);

private:
	struct
	{
		DynamicArray<const VulkanVertexBuffer*> vertexBuffers;
		DynamicArray<u32> vertexBufferOffsets;
	} vertexBuffersAndOffsets;

	static constexpr u32 frameTempAllocCount = 2;
	VulkanFrameTempAllocation* frameTempAlloc[frameTempAllocCount];
	VulkanFrameTempAllocation* currentFrameTempAlloc = nullptr;

	// TODO - It sort of feels weird to have this exist in the renderer. Maybe it should, I don't know
	NativeTexture* backBuffer = nullptr;

	VulkanDevice& logicalDevice;

	FixedArray<ColorDescription, MaxColorTargetCount> currentColorTargets;
	DepthStencilDescription currentDepthTarget;
	VulkanRenderState currentRenderState;
	VkViewport viewport;
	// TODO - Isn't always used or enabled...
	VkRect2D  scissorRect;
	// TODO - Might be better to split this update up into each "dirty" element, like viewport but not scissor
	bool updateViewState;
};