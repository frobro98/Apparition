#pragma once

#include "BasicTypes/Color.hpp"
#include "BasicTypes/Intrinsics.hpp"
#include "Image.h"

namespace AptnLoadOperation
{
enum Type
{
	Load,
	Clear,
	DontCare,

	Count = DontCare + 1
};
}// LoadOperation
static_assert(AptnLoadOperation::Count < 4, "LoadOperation must be less that 3 bit");

namespace AptnStoreOperation
{
enum Type
{
	Store,
	DontCare,

	Count = DontCare + 1
};
}// StoreOperation
static_assert(AptnStoreOperation::Count < 4, "StoreOperation must be less that 3 bit");

#define ATTACHMENT_OP_MASK 2
#define CREATE_ATTACHMENT_OP(LoadOp, StoreOp) (((u8)AptnLoadOperation::LoadOp << ATTACHMENT_OP_MASK) | ((u8)AptnStoreOperation::StoreOp))
namespace AptnAttachmentOperations
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

constexpr AptnLoadOperation::Type LoadOperationFrom(AptnAttachmentOperations::Type op)
{
	return (AptnLoadOperation::Type)(op >> ATTACHMENT_OP_MASK);
}

constexpr AptnStoreOperation::Type StoreOperationFrom(AptnAttachmentOperations::Type op)
{
	return (AptnStoreOperation::Type)(op & ((1 << ATTACHMENT_OP_MASK) - 1));
}

constexpr AptnAttachmentOperations::Type AttachmentOperationsFrom(AptnLoadOperation::Type loadOp, AptnStoreOperation::Type storeOp)
{
	return (AptnAttachmentOperations::Type)((loadOp << ATTACHMENT_OP_MASK) | (storeOp));
}

#undef ATTACHMENT_OP_MASK
#undef CREATE_ATTACHMENT_OP

union AptnAttachmentClearValue
{
	Color32 color;
	struct AptnDepthStencilValue
	{
		float depth;
		u32 stencil;
	} depthStencil;
};

// TODO: Move this somewhere else
struct AptnRenderAttachment
{
	AptnImageView imageView;
	AptnAttachmentOperations::Type loadStoreOps;
	AptnAttachmentClearValue clearValue;
};

struct AptnRenderSetupParams
{
	DynamicArray<AptnRenderAttachment> colorAttachments;
	AptnRenderAttachment depthAttachment;
	AptnRenderAttachment stencilAttachment;
	u32 renderWidth = 0;
	u32 renderHeight = 0;
};
