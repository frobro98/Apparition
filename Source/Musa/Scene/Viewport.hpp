// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Engine/FrameRenderTargets.hpp"
#include "MusaAPI.hpp"

class MUSA_API Viewport final
{
public:
	Viewport(void* windowHandle, u32 width, u32 height);
	~Viewport();

	void Resize(u32 newWidth, u32 newHeight);

	forceinline FrameRenderTargets& GetRenderTargets() { return renderTargets; }
	forceinline NativeViewport& GetNativeViewport() const { return *graphicsViewport; }
	forceinline u32 GetWidth() const { return viewWidth; }
	forceinline u32 GetHeight() const { return viewHeight; }

private:
	void InitializeRenderTargets();

private:
	FrameRenderTargets renderTargets;
	NativeViewport* graphicsViewport = nullptr;
	i32 viewWidth;
	i32 viewHeight;
};
