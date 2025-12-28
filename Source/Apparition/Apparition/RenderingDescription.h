#pragma once

#include "BasicTypes/Color.hpp"
#include "BasicTypes/Intrinsics.hpp"
#include "Image.h"

namespace Apparition
{
enum class LoadOperation : u8
{
	Load,
	Clear,
	DontCare
};

// TODO - move this enum to a better place
enum class StoreOperation : u8
{
	Store,
	DontCare
};

#define ATTACHMENT_OP_MASK 2
#define CREATE_ATTACHMENT_OP(LoadOp, StoreOp) (((u8)LoadOperation::LoadOp << ATTACHMENT_OP_MASK) | ((u8)StoreOperation::StoreOp))
// TODO - These ops aren't necessarily for Color Targets. Should they be RenderTarget operations?
enum class RenderAttachmentOperations : u8
{
	DontLoad_DontStore = CREATE_ATTACHMENT_OP(DontCare, DontCare),
	DontLoad_Store = CREATE_ATTACHMENT_OP(DontCare, Store),
	Load_DontStore = CREATE_ATTACHMENT_OP(Load, DontCare),
	Load_Store = CREATE_ATTACHMENT_OP(Load, Store),
	Clear_DontStore = CREATE_ATTACHMENT_OP(Clear, DontCare),
	Clear_Store = CREATE_ATTACHMENT_OP(Clear, Store)
};

constexpr LoadOperation GetLoadOperation(RenderAttachmentOperations op)
{
	return (LoadOperation)((u8)op >> ATTACHMENT_OP_MASK);
}

constexpr StoreOperation GetStoreOperation(RenderAttachmentOperations op)
{
	return (StoreOperation)((u8)op & ((1 << ATTACHMENT_OP_MASK) - 1));
}

constexpr RenderAttachmentOperations CreateRenderAttachmentOperations(LoadOperation loadOp, StoreOperation storeOp)
{
	return (RenderAttachmentOperations)(((u8)loadOp << ATTACHMENT_OP_MASK) | ((u8)storeOp));
}

#undef ATTACHMENT_OP_MASK
#undef CREATE_ATTACHMENT_OP

union AttachmentClearValue
{
	Color32 color;
	struct DepthStencilValue
	{
		float depth;
		u32 stencil;
	} depthStencil;
};

// TODO: Move this somewhere else
struct RenderAttachment
{
	ImageView imageView;
	RenderAttachmentOperations loadStoreOps;
	AttachmentClearValue clearValue;
};

struct RenderSetupParams
{
	DynamicArray<RenderAttachment> colorAttachments;
	RenderAttachment depthAttachment;
	RenderAttachment stencilAttachment;
	u32 renderWidth = 0;
	u32 renderHeight = 0;
};
}