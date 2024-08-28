// Copyright 2020, Nathan Blane

#pragma once

#include "GraphicsResourceFlags.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Texture/ImageFormats.h"
#include "RenderPassAttachments.hpp"
#include "Containers/DynamicArray.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Graphics/GraphicsAPI.hpp"

struct Vertex;
struct Face;
class ResourceBlob;
class RenderContext;
struct SamplerDescription;

class GRAPHICS_API GraphicsInterface
{
public:

	// TODO - Decide on whether or not to put the Begin/EndFrame calls in this interface or a different interface more focused on the actual render pipeline

	virtual ~GraphicsInterface() = default;

	virtual void InitializeGraphics() = 0;
	virtual void DeinitializeGraphics() = 0;
	
	NODISCARD virtual NativeVertexShader* CreateVertexShader(const MemoryBuffer& vertexCode) = 0;
	NODISCARD virtual NativeFragmentShader* CreateFragmentShader(const MemoryBuffer& fragmentCode) = 0;
	NODISCARD virtual NativeGeometryShader* CreateGeometryShader(const MemoryBuffer& geometryCode) = 0;
	NODISCARD virtual NativeTessEvaluationShader* CreateTessEvaluationShader(const MemoryBuffer& tessEvalCode) = 0;
	NODISCARD virtual NativeTessControlShader* CreateTessControlShader(const MemoryBuffer& tessCtrlCode) = 0;
	NODISCARD virtual NativeComputeShader* CreateComputeShader(const MemoryBuffer& computeCode) = 0;

	virtual void DestroyVertexShader(const NativeVertexShader* vs) = 0;
	virtual void DestroyFragmentShader(const NativeFragmentShader* fs) = 0;
	virtual void DestroyGeometryShader(const NativeGeometryShader* gs) = 0;
	virtual void DestroyTessEvaluationShader(const NativeTessEvaluationShader* te) = 0;
	virtual void DestroyTessControlShader(const NativeTessControlShader* tc) = 0;
	virtual void DestroyComputeShader(const NativeComputeShader* cs) = 0;

	NODISCARD virtual NativeViewport* CreateViewport(void* windowHandle, u32 viewWidth, u32 viewHeight) = 0;
	virtual void ResizeViewport(NativeViewport& viewport, u32 newViewWidth, u32 newViewHeight) = 0;
	virtual void DestroyViewport(const NativeViewport* viewport) = 0;

	NODISCARD virtual NativeVertexBuffer* CreateVertexBuffer(u64 sizeInBytes, const ResourceBlob* blob = nullptr) const = 0;
	NODISCARD virtual NativeIndexBuffer* CreateIndexBuffer(u64 sizeInBytes, u32 indexSize, const ResourceBlob* blob = nullptr) const = 0;
	NODISCARD virtual NativeUniformBuffer* CreateUniformBuffer(u32 bufferSize) const = 0;

	virtual void DestroyVertexBuffer(const NativeVertexBuffer* vb) const = 0;
	virtual void DestroyIndexBuffer(const NativeIndexBuffer* ib) const = 0;
	virtual void DestroyUniformBuffer(const NativeUniformBuffer* ub) const = 0;

	NODISCARD virtual NativeTexture* CreateEmptyTexture2D(u32 width, u32 height, ImageFormat textureFormat, u32 mipLevels, TextureUsage::Type usage) = 0;
	NODISCARD virtual NativeTexture* CreateInitializedTexture2D(const ResourceBlob& textureBlob, u32 width, u32 height, ImageFormat textureFormat, u32 mipLevels, TextureUsage::Type usage) = 0;
	NODISCARD virtual NativeSampler* CreateTextureSampler(const SamplerDescription& params) = 0;

	virtual void DestroyTexture(const NativeTexture* tex) = 0;

	NODISCARD virtual void* GetGraphicsDevice() = 0;
	NODISCARD virtual RenderContext* GetRenderContext() = 0;
	
	// TODO - Consider locking/unlocking, similar to how VBs and IBs are treated
	virtual void PushTextureData(NativeTexture& texture, const ResourceBlob& textureBlob) = 0;
	// TODO - Consider locking/unlocking, similar to how VBs and IBs are treated
	virtual void PushBufferData(NativeUniformBuffer& buffer, const void* data) const = 0;

	NODISCARD virtual void* LockVertexBuffer(NativeVertexBuffer* vb, u64 size, u32 lockOffset) = 0;
	virtual void UnlockVertexBuffer(NativeVertexBuffer* vb) = 0;

	NODISCARD virtual void* LockIndexBuffer(NativeIndexBuffer* vb, u64 size, u32 lockOffset) = 0;
	virtual void UnlockIndexBuffer(NativeIndexBuffer* vb) = 0;
};

GRAPHICS_API GraphicsInterface& GetGraphicsInterface();
