#pragma once

#include "BasicTypes/Color.hpp"
#include "BasicTypes/Intrinsics.hpp"
#include "Image.h"

enum class AptnLoadOperation : u8
{
	Load,
	Clear,
	DontCare,

	Count = DontCare + 1
};
static_assert((__underlying_type(AptnLoadOperation))AptnLoadOperation::Count < 4, "LoadOperation must be less that 3 bit");

enum class AptnStoreOperation : u8
{
	Store,
	DontCare,

	Count = DontCare + 1
};
static_assert((__underlying_type(AptnStoreOperation))AptnStoreOperation::Count < 4, "StoreOperation must be less that 3 bit");

#define ATTACHMENT_OP_MASK 2
#define CREATE_ATTACHMENT_OP(LoadOp, StoreOp) (((u8)AptnLoadOperation::LoadOp << ATTACHMENT_OP_MASK) | ((u8)AptnStoreOperation::StoreOp))
enum class AptnAttachmentOperations
{
	DontLoad_DontStore = CREATE_ATTACHMENT_OP(DontCare, DontCare),
	DontLoad_Store = CREATE_ATTACHMENT_OP(DontCare, Store),
	Load_DontStore = CREATE_ATTACHMENT_OP(Load, DontCare),
	Load_Store = CREATE_ATTACHMENT_OP(Load, Store),
	Clear_DontStore = CREATE_ATTACHMENT_OP(Clear, DontCare),
	Clear_Store = CREATE_ATTACHMENT_OP(Clear, Store)
};

constexpr AptnLoadOperation LoadOperationFrom(AptnAttachmentOperations op)
{
	return (AptnLoadOperation)((__underlying_type(AptnAttachmentOperations))op >> ATTACHMENT_OP_MASK);
}

constexpr AptnStoreOperation StoreOperationFrom(AptnAttachmentOperations op)
{
	return (AptnStoreOperation)((__underlying_type(AptnAttachmentOperations))op & ((1 << ATTACHMENT_OP_MASK) - 1));
}

constexpr AptnAttachmentOperations AttachmentOperationsFrom(AptnLoadOperation loadOp, AptnStoreOperation storeOp)
{
	return (AptnAttachmentOperations)(((__underlying_type(AptnLoadOperation))loadOp << ATTACHMENT_OP_MASK) | ((__underlying_type(AptnStoreOperation))storeOp));
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
	AptnAttachmentOperations loadStoreOps;
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
