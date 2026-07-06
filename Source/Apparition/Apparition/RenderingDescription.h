#pragma once

#include "BasicTypes/Color.hpp"
#include "BasicTypes/Intrinsics.hpp"
#include "Image.h"

namespace Apparition
{
namespace LoadOperation
{
enum Type
{
	Load,
	Clear,
	DontCare,

	Count = DontCare + 1
};
}// LoadOperation
static_assert(LoadOperation::Count < 4, "LoadOperation must be less that 3 bit");

namespace StoreOperation
{
enum Type
{
	Store,
	DontCare,

	Count = DontCare + 1
};
}// StoreOperation
static_assert(StoreOperation::Count < 4, "StoreOperation must be less that 3 bit");

#define ATTACHMENT_OP_MASK 2
#define CREATE_ATTACHMENT_OP(LoadOp, StoreOp) (((u8)LoadOperation::LoadOp << ATTACHMENT_OP_MASK) | ((u8)StoreOperation::StoreOp))
namespace AttachmentOperations
{
enum Type
{
	DontLoad_DontStore = CREATE_ATTACHMENT_OP(DontCare, DontCare),
	DontLoad_Store = CREATE_ATTACHMENT_OP(DontCare, Store),
	Load_DontStore = CREATE_ATTACHMENT_OP(Load, DontCare),
	Load_Store = CREATE_ATTACHMENT_OP(Load, Store),
	Clear_DontStore = CREATE_ATTACHMENT_OP(Clear, DontCare),
	Clear_Store = CREATE_ATTACHMENT_OP(Clear, Store)
};
}// AttachmentOperations

constexpr LoadOperation::Type LoadOperationFrom(AttachmentOperations::Type op)
{
	return (LoadOperation::Type)(op >> ATTACHMENT_OP_MASK);
}

constexpr StoreOperation::Type StoreOperationFrom(AttachmentOperations::Type op)
{
	return (StoreOperation::Type)(op & ((1 << ATTACHMENT_OP_MASK) - 1));
}

constexpr AttachmentOperations::Type AttachmentOperationsFrom(LoadOperation::Type loadOp, StoreOperation::Type storeOp)
{
	return (AttachmentOperations::Type)((loadOp << ATTACHMENT_OP_MASK) | (storeOp));
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
	AttachmentOperations::Type loadStoreOps;
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