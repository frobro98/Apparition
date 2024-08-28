// Copyright 2020, Nathan Blane

#include "RenderTarget.hpp"

GRAPHICS_API FixedArray<ColorDescription, MaxColorTargetCount> CreateColorTargetDescriptions(const FixedArray<const RenderTarget*, MaxColorTargetCount>& colorTargets, RenderTargetAccess access)
{
	FixedArray<ColorDescription, MaxColorTargetCount> colorDescs(colorTargets.Size());
	for (u32 i = 0; i < colorDescs.Size(); ++i)
	{
		const RenderTarget* target = colorTargets[i];
		ColorDescription& desc = colorDescs[i];
		desc.access = access;
		desc.format = target->format;
		desc.op = CreateColorTargetOperations(target->load, target->store);
		desc.sampleCount = target->sampleCount;
	}

	return colorDescs;
}

GRAPHICS_API DepthStencilDescription CreateDepthTargetDescription(const RenderTarget* depthTarget, RenderTargetAccess access)
{
	DepthStencilDescription depthDesc = {};
	depthDesc.access = access;
	depthDesc.format = depthTarget->format;
	depthDesc.op = CreateDepthStencilOperations(
		CreateColorTargetOperations(depthTarget->load, depthTarget->store),
		CreateColorTargetOperations(depthTarget->stencilLoad, depthTarget->stencilStore)
		);
	depthDesc.sampleCount = depthTarget->sampleCount;

	return depthDesc;
}

NativeRenderTargets CreateNativeRenderTargets(const FixedArray<const RenderTarget*, MaxColorTargetCount>& colorTargets, const RenderTarget * depthTarget)
{
	// TODO - Check can be made in a more compile time way
	AssertFunc([&] {
		u32 width = colorTargets[0]->width;
		u32 height = colorTargets[0]->height;
		for (u32 i = 1; i < colorTargets.Size(); ++i)
		{
			const RenderTarget* rt = colorTargets[i];
			if (width != rt->width ||
				height != rt->height)
				return false;
		}
		return true;
		}, "Dimensions aren't the same for each target!");

	NativeRenderTargets targets = {};
	u32 numColorTargets = colorTargets.Size();
	targets.colorTargets.Resize(numColorTargets);
	for (u32 i = 0; i < numColorTargets; ++i)
	{
		targets.colorTargets[i] = colorTargets[i]->nativeTarget;
	}

	if (depthTarget != nullptr)
	{
		targets.depthTarget = depthTarget->nativeTarget;
	}

	targets.extents = { (f32)colorTargets[0]->width, (f32)colorTargets[0]->height };

	return targets;
}
