// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"
#include "GraphicsInterface.hpp"
#include "Graphics/RenderContext.hpp"
#include "VulkanDevice.h"


class VulkanGraphicsInterface final : public GraphicsInterface
{
public:
	virtual void InitializeGraphics() override;
	virtual void DeinitializeGraphics() override;

	virtual NativeVertexShader* CreateVertexShader(const MemoryBuffer& vertexCode) override;
	virtual NativeFragmentShader* CreateFragmentShader(const MemoryBuffer& fragmentCode) override;
	virtual NativeGeometryShader* CreateGeometryShader(const MemoryBuffer& geometryCode) override;
	virtual NativeTessEvaluationShader* CreateTessEvaluationShader(const MemoryBuffer& tessEvalCode) override;
	virtual NativeTessControlShader* CreateTessControlShader(const MemoryBuffer& tessCtrlCode) override;
	virtual NativeComputeShader* CreateComputeShader(const MemoryBuffer& computeCode) override;

	virtual void DestroyVertexShader(const NativeVertexShader* vs) override;
	virtual void DestroyFragmentShader(const NativeFragmentShader* fs) override;
	virtual void DestroyGeometryShader(const NativeGeometryShader* gs) override;
	virtual void DestroyTessEvaluationShader(const NativeTessEvaluationShader* te) override;
	virtual void DestroyTessControlShader(const NativeTessControlShader* tc) override;
	virtual void DestroyComputeShader(const NativeComputeShader* cs) override;

	virtual NativeViewport* CreateViewport(void* windowHandle, u32 viewWidth, u32 viewHeight) override;
	virtual void ResizeViewport(NativeViewport& viewport, u32 newViewWidth, u32 newViewHeight) override;
	virtual void DestroyViewport(const NativeViewport* viewport) override;

	virtual NativeVertexBuffer* CreateVertexBuffer(u64 sizeInBytes, const ResourceBlob* blob) const override;
	virtual NativeIndexBuffer* CreateIndexBuffer(u64 sizeInBytes, u32 indexSize, const ResourceBlob* blob = nullptr) const override;
	virtual NativeUniformBuffer* CreateUniformBuffer(u32 bufferSize) const override;

	virtual void DestroyVertexBuffer(const NativeVertexBuffer* vb) const override;
	virtual void DestroyIndexBuffer(const NativeIndexBuffer* ib) const override;
	virtual void DestroyUniformBuffer(const NativeUniformBuffer* ub) const override;

	virtual NativeTexture* CreateEmptyTexture2D(u32 width, u32 height, ImageFormat textureFormat, u32 mipLevels, TextureUsage::Type usage) override;
	virtual NativeTexture* CreateInitializedTexture2D(const ResourceBlob& textureBlob, u32 width, u32 height, ImageFormat textureFormat, u32 mipLevels, TextureUsage::Type usage) override;
	virtual NativeSampler* CreateTextureSampler(const SamplerDescription& params) override;

	virtual void DestroyTexture(const NativeTexture* tex) override;

	virtual void* GetGraphicsDevice() override;
	virtual RenderContext* GetRenderContext() override;

	virtual void PushTextureData(NativeTexture& texture, const ResourceBlob& textureBlob) override;
	virtual void PushBufferData(NativeUniformBuffer& buffer, const void* data) const override;

	virtual void* LockVertexBuffer(NativeVertexBuffer* vb, u64 size, u32 lockOffset) override;
	virtual void UnlockVertexBuffer(NativeVertexBuffer* vb) override;

	virtual void* LockIndexBuffer(NativeIndexBuffer* vb, u64 size, u32 lockOffset) override;
	virtual void UnlockIndexBuffer(NativeIndexBuffer* vb) override;

private:
	void CreateInstance();
	void SetupDebugUtilsFunctions();

private:
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;
	//VkDebugReportCallbackEXT debugReportHandle = VK_NULL_HANDLE;
	VkInstance instance = VK_NULL_HANDLE;

	UniquePtr<RenderContext> renderContext;
	UniquePtr<VulkanDevice> logicalDevice;
};
