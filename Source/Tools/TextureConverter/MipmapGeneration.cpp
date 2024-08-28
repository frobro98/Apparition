// Copyright 2020, Nathan Blane

#include "MipmapGeneration.hpp"
#include "FastFourierTransform.hpp"
#include "Texture2D/Texture.h"
#include "Math/MathFunctions.hpp"
#include "BasicTypes/Color.hpp"
#include "Texture2D/TextureUtilities.hpp"

namespace
{
struct ImageDividedChannels
{
	DynamicArray<u8> redChannel;
	DynamicArray<u8> greenChannel;
	DynamicArray<u8> blueChannel;
};

void SplitTextureIntoChannels(const MipmapLevel& texture, ImageDividedChannels& image)
{
	constexpr u32 numChannels = 4;
	const u32 rowSize = texture.width * numChannels;
	const u8* mipData = texture.mipData.GetData();

	image.redChannel.Reserve(texture.width * texture.height);
	image.greenChannel.Reserve(texture.width * texture.height);
	image.blueChannel.Reserve(texture.width * texture.height);
	for (u32 i = 0; i < texture.height; ++i)
	{
		for (u32 j = 0; j < texture.width * numChannels; j += 4)
		{
			image.redChannel.Add(mipData[(j + rowSize * i) + 0]);
			image.greenChannel.Add(mipData[(j + rowSize * i) + 1]);
			image.blueChannel.Add(mipData[(j + rowSize * i) + 2]);
		}
	}
}

void CombineImageChannels(const ImageDividedChannels& image, MipmapLevel& level)
{
	constexpr u32 numChannels = 4;
	const u32 rowSize = level.width * numChannels;
	const u32 mipLevelSize = rowSize * level.height;
	u8* mipData = new u8[mipLevelSize];
	ZeroMem(mipData, mipLevelSize);

	u8* iter = mipData;
	for (u32 i = 0; i < level.height; ++i)
	{
		for (u32 j = 0; j < level.width; ++j)
		{
			*iter++ = (image.redChannel[j + level.width * i]);
			*iter++ = (image.greenChannel[j + level.width * i]);
			*iter++ = (image.blueChannel[j + level.width * i]);
			*iter++ = ((u8)255);
		}
	}

	level.mipData = ResourceBlob(mipData, mipLevelSize);
}

constexpr u32 GoodKaiserWidth = 32;

float bessel0Function(float x)
{
	const float EPSILON_RATIO = 1E-8f;
	float xh, sum, pow, ds;

	xh = .5f * x;
	sum = 1.f;
	pow = 1.f;
	ds = 1.f;

	int k = 0;
	while (ds > sum * EPSILON_RATIO)
	{
		k++;
		pow *= (xh / k);
		ds = pow * pow;
		sum = sum + ds;
	}

	return sum;
}

float kaiserFunction(float alpha, float halfWidth, float x)
{
	float ratio = (x / halfWidth);
	float kaiser = bessel0Function(alpha * Math::Sqrt(1 - ratio * ratio)) / bessel0Function(alpha);
	return kaiser;
}

template<u32 N>
struct ImageFilter
{
	float data[N] = {};
};

template<u32 N>
void NormalizeFilter(ImageFilter<N>& filter)
{
	float sum = 0.f;

	for (u32 i = 0; i < N; ++i)
	{
		sum += filter.data[i];
	}

	Assert(sum != 0);

	for (u32 i = 0; i < N; ++i)
	{
		filter.data[i] /= sum;
	}
}

ImageFilter<3> InitializeBoxFilter()
{
	ImageFilter<3> boxFilter = {};
	boxFilter.data[0] = 1;
	boxFilter.data[1] = 2;
	boxFilter.data[2] = 1;
	NormalizeFilter(boxFilter);

	return boxFilter;
}

ImageFilter<GoodKaiserWidth> InitializeKaiserFilter()
{
	ImageFilter<GoodKaiserWidth> kaiserFilter = {};
	const float halfWidth = GoodKaiserWidth * .5f;
	const float offset = -halfWidth;
	const float nudge = .5f;
	const float stretch = 1.f;
	const float alpha = 4.f;

	for (u32 i = 0; i < GoodKaiserWidth; ++i)
	{
		const float x = (i + offset) + nudge;
		const float xStretchAdjust = x * stretch;
		const float sincValue = Math::Sinc(xStretchAdjust);
		const float windowValue = kaiserFunction(alpha, halfWidth, xStretchAdjust);

		kaiserFilter.data[i] = sincValue * windowValue;
	}

	NormalizeFilter(kaiserFilter);

	return kaiserFilter;
}

u32 Get1DColorIndex(i32 i, i32 j, u32 width, u32 height)
{
	Assert(i >= 0 && i < ((i32)width));
	Assert(j >= 0 && j < ((i32)height));

	return static_cast<u32>(i + j * width);
}

u32 AdjustIndexforFiltering(i32 i, i32 j, u32 width, u32 height)
{
	while ((i < 0) || (i > ((i32)width - 1)))
	{
		if (i < 0) i = -i;
		if (i >= (i32)(width)) i = (i32)(width + width) - i - 1;
	}

	while ((j < 0) || (j > ((i32)height - 1)))
	{
		if (j < 0) j = -j;
		if (j >= (i32)height) j = (i32)(height + height) - j - 1;
	}

	return Get1DColorIndex(i, j, width, height);
}

// template<uint32 N>
// void ApplyFilterToLevel(const ImageFilter<N>& filter, const MipMapLevel& parent, MipMapLevel& level)

template<u32 N>
void ApplyFilterToLevel(const ImageFilter<N>& filter,u32 parentWidth, u32 parentHeight, const DynamicArray<u8>& channelData, DynamicArray<u8>& processedChannelData)
{
	static_assert(N != 1, "Width of a filter must be usable");

	DynamicArray<float> normalizedPixelData = NormalizePixelData(channelData);
	DynamicArray<float> tmpData(normalizedPixelData.Size());
	i32 filterOffset = -static_cast<i32>((N / 2) - 1);

	u32 levelWidth = parentWidth >> 1;
	u32 levelHeight = parentHeight >> 1;
	DynamicArray<float> normalizedResult(levelWidth * levelHeight);

	for (i32 y = 0; y < (i32)parentHeight; ++y)
	{
		for (i32 x = 0; x < (i32)parentWidth; x += 2)
		{
			u32 resultingIndex = Get1DColorIndex(x >> 1, y, parentWidth, parentHeight);
			float sum = 0;

			for (i32 k = 0; k < N; ++k)
			{
				i32 srcX = x + k + filterOffset;
				u32 colorIndex = AdjustIndexforFiltering(srcX, y, parentWidth, parentHeight);
				const float color = normalizedPixelData[colorIndex];
				sum += filter.data[k] * color;
			}

			tmpData[resultingIndex] = sum;
		}
	}

	u32 numColorsTouched = 0;
	for (i32 x = 0; x < (i32)(parentWidth >> 1); ++x)
	{
		for (i32 y = 0; y < (i32)parentHeight; y += 2)
		{
			u32 resultingIndex = Get1DColorIndex(x, y >> 1, levelWidth, levelHeight);
			float sum = 0;

			for (i32 k = 0; k < N; ++k)
			{
				i32 srcY = y + k + filterOffset;
				u32 colorIndex = AdjustIndexforFiltering(x, srcY, parentWidth, parentHeight);
				const float channelColor = tmpData[colorIndex];
				sum += filter.data[k] * channelColor;
			}

			normalizedResult[resultingIndex] = sum;
			++numColorsTouched;
		}
	}

	processedChannelData = UnnormalizePixelData(normalizedResult);
}

template<u32 N>
MipmapLevel ProcessIndividualLevel(const ImageFilter<N>& filter, const MipmapLevel& parentLevel)
{
	MipmapLevel currentLevel;
	ImageDividedChannels parentImage;
	SplitTextureIntoChannels(parentLevel, parentImage);

	currentLevel.width = parentLevel.width >> 1;
	currentLevel.height = parentLevel.height >> 1;

	ImageDividedChannels processedImage;
	ApplyFilterToLevel(filter, parentLevel.width, parentLevel.height, parentImage.redChannel, processedImage.redChannel);
	ApplyFilterToLevel(filter, parentLevel.width, parentLevel.height, parentImage.greenChannel, processedImage.greenChannel);
	ApplyFilterToLevel(filter, parentLevel.width, parentLevel.height, parentImage.blueChannel, processedImage.blueChannel);

	CombineImageChannels(processedImage, currentLevel);

	return currentLevel;
}

void ApplyFFTToLevel(u32 parentWidth, u32 parentHeight, const DynamicArray<u8>& channelData, DynamicArray<u8>& processedChannel)
{
	DynamicArray<float> normalizedPixelData = NormalizePixelData(channelData);
	
	u32 pixelDataSize = normalizedPixelData.Size();
	DynamicArray<float> tmpReal(pixelDataSize);
	DynamicArray<float> tmpImaginary(pixelDataSize);
	DynamicArray<float> freqReal(pixelDataSize);
	DynamicArray<float> freqImaginary(pixelDataSize);

	u32 maxDimension = parentWidth;
	if (parentHeight > maxDimension)
	{
		maxDimension = parentHeight;
	}

	DynamicArray<float> inputReal(maxDimension);
	DynamicArray<float> outputReal(maxDimension);
	DynamicArray<float> inputImaginary(maxDimension);
	DynamicArray<float> outputImaginary(maxDimension);

	// Do the two-dimensional forward FFT.  To accomplish
	// this we first do a horizontal pass, then a vertical
	// pass.

	i32 parentWidthAsInt = static_cast<i32>(parentWidth);
	i32 parentHeightAsInt = static_cast<i32>(parentHeight);
	// Horizontal pass.
	for (i32 y = 0; y < parentHeightAsInt; ++y)
	{
		for (i32 x = 0; x < parentWidthAsInt; ++x)
		{
			u32 src = static_cast<u32>(x);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			inputReal[src] = normalizedPixelData[index];
			inputImaginary[src] = 0.f;
		}

		FastFourier(parentWidth, inputReal, inputImaginary, outputReal, outputImaginary);

		for (i32 x = 0; x < parentWidthAsInt; ++x)
		{
			u32 dest = static_cast<u32>(x);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			tmpReal[index] = outputReal[dest];
			tmpImaginary[index] = outputImaginary[dest];
		}
	}

	// Vertical Pass

	for (i32 x = 0; x < parentWidthAsInt; ++x)
	{
		for (i32 y = 0; y < parentHeightAsInt; ++y)
		{
			u32 src = static_cast<u32>(y);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			inputReal[src] = tmpReal[index];
			inputImaginary[src] = tmpImaginary[index];
		}

		FastFourier(parentHeight, inputReal, inputImaginary, outputReal, outputImaginary);

		for (i32 y = 0; y < parentHeightAsInt; ++y)
		{
			u32 dest = static_cast<u32>(y);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			freqReal[index] = outputReal[dest];
			freqImaginary[index] = outputImaginary[dest];
		}
	}

	i32 halfwidth = parentWidthAsInt / 2;
	i32 quarterwidth = parentWidthAsInt / 4;

	i32 halfheight = parentHeightAsInt / 2;
	i32 quarterheight = parentHeightAsInt / 4;

	i32 x0 = quarterwidth;
	i32 x1 = halfwidth + quarterwidth;
	i32 y0 = quarterheight;
	i32 y1 = halfheight + quarterheight;

	for (i32 y = 0; y < parentHeightAsInt; ++y)
	{
		for (i32 x = 0; x < parentWidthAsInt; ++x)
		{
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			if ((x >= x0 && x < x1) || (y >= y0 && y < y1))
			{
				freqReal[index] = 0;
				freqImaginary[index] = 0;
			}
		}
	}

	// Reverse FFT.  For kicks we do vertical pass first,
	// then horizontal pass, though the order should not matter.

	// Vertical pass.
	for (i32 x = 0; x < parentWidthAsInt; ++x)
	{
		for (i32 y = 0; y < parentHeightAsInt; ++y)
		{
			u32 src = static_cast<u32>(y);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			inputReal[src] = freqReal[index];
			inputImaginary[src] = freqImaginary[index];
		}

		InverseFastFourier(parentHeight, inputReal, inputImaginary, outputReal, outputImaginary);

		for (i32 y = 0; y < parentHeightAsInt; ++y)
		{
			u32 dest = static_cast<u32>(y);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			tmpReal[index] = outputReal[dest];
			tmpImaginary[index] = outputImaginary[dest];
		}
	}

	// Horizontal Pass
	for (i32 y = 0; y < parentHeightAsInt; ++y)
	{
		for (i32 x = 0; x < parentWidthAsInt; ++x)
		{
			u32 src = static_cast<u32>(x);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			inputReal[src] = tmpReal[index];
			inputImaginary[src] = tmpImaginary[index];
		}

		InverseFastFourier(parentWidth, inputReal, inputImaginary, outputReal, outputImaginary);

		for (i32 x = 0; x < parentWidthAsInt; ++x)
		{
			u32 dest = static_cast<u32>(x);
			u32 index = Get1DColorIndex(x, y, parentWidth, parentHeight);
			tmpReal[index] = outputReal[dest];
			// outputImaginary is discarded.  Should be 0 within roundoff error.
		}
	}

	u32 levelWidth = parentWidth >> 1;
	u32 levelHeight = parentHeight >> 1;
	DynamicArray<float> normalizedResult(levelWidth * levelHeight);

	for (i32 y = 0; y < parentHeightAsInt; y += 2)
	{
		for (i32 x = 0; x < parentWidthAsInt; ++x)
		{
			u32 srcIndex = Get1DColorIndex(x, y, parentWidth, parentHeight);
			u32 destIndex = Get1DColorIndex(x >> 1, y >> 1, levelWidth, levelHeight);
			normalizedResult[destIndex] = tmpReal[srcIndex];
		}
	}

	processedChannel = UnnormalizePixelData(normalizedResult);

}

MipmapLevel ProcessIndividualLevelWithFFT(const MipmapLevel& parentLevel)
{
	MipmapLevel currentLevel;
	currentLevel.width = parentLevel.width >> 1;
	currentLevel.height = parentLevel.height >> 1;

	ImageDividedChannels parentImage;
	SplitTextureIntoChannels(parentLevel, parentImage);

	ImageDividedChannels processedImage;
	ApplyFFTToLevel(parentLevel.width, parentLevel.height, parentImage.redChannel, processedImage.redChannel);
	ApplyFFTToLevel(parentLevel.width, parentLevel.height, parentImage.greenChannel, processedImage.greenChannel);
	ApplyFFTToLevel(parentLevel.width, parentLevel.height, parentImage.blueChannel, processedImage.blueChannel);

	CombineImageChannels(processedImage, currentLevel);

	return currentLevel;
}

template<u32 N>
void ProcessMipLevels(const ImageFilter<N>& filter, DynamicArray<MipmapLevel>& mipMapLevels, u32 levelsToGen)
{
	const MipmapLevel* parentLevel = &mipMapLevels[0];
	for (u32 i = 1; i < levelsToGen; ++i)
	{
		mipMapLevels.Add(ProcessIndividualLevel(filter, *parentLevel));
		parentLevel = &mipMapLevels[i];
	}
}

static auto boxFilter = InitializeBoxFilter();
static auto kaiserFilter = InitializeKaiserFilter();

void ProcessMipLevelsWithBoxFilter(DynamicArray<MipmapLevel>& mipMapLevels, u32 levelsToGen)
{
	const auto& filter = boxFilter;
	ProcessMipLevels(filter, mipMapLevels, levelsToGen);
}

void ProcessMipLevelsWithKaiserFilter(DynamicArray<MipmapLevel>& mipMapLevels, u32 levelsToGen)
{
	const auto& filter = kaiserFilter;
	ProcessMipLevels(filter, mipMapLevels, levelsToGen);
}

void ProcessMipLevelsWithFFT(DynamicArray<MipmapLevel>& mipMapLevels, u32 levelsToGen)
{
	const MipmapLevel* parentLevel = &mipMapLevels[0];
	for (u32 i = 1; i < levelsToGen; ++i)
	{
		mipMapLevels.Add(ProcessIndividualLevelWithFFT(*parentLevel));
		parentLevel = &mipMapLevels[i];
	}
}

}

u32 GetMaxMipMapLevels(u32 width, u32 height)
{
	float maxDimention = static_cast<float>(Math::Max(width, height));
	u32 numPossibleLevels = 1 + static_cast<u32>(Math::Floor(Math::Log2(maxDimention)));
	return numPossibleLevels;
}

bool GenerateMipMapLevels(Texture& texture, u32 levelsToGen, MipMapFilter filter)
{
	Assert(texture.mipLevels.Size() > 0);
	u32 width = texture.GetWidth();
	u32 height = texture.GetHeight();
	Assert(levelsToGen < GetMaxMipMapLevels(width, height));

	switch (filter)
	{
		case MipMapFilter::Box:	
		{
			ProcessMipLevelsWithBoxFilter(texture.mipLevels, levelsToGen);
		}break;
		case MipMapFilter::Kaiser:
		{
			ProcessMipLevelsWithKaiserFilter(texture.mipLevels, levelsToGen);
		}break;
		case MipMapFilter::FFT:
		{
			ProcessMipLevelsWithFFT(texture.mipLevels, levelsToGen);
		}break;
	}

	return true;
}

/*
// BOX FILTERING?
MipMapLevel currentLevel;
uint32 parentWidth = parentLevel->width;
uint32 parentHeight = parentLevel->height;
width = parentWidth >> 1;
height = parentHeight >> 1;

for (uint32 y = 0; y < height * numColorComponents; y += numColorComponents)
{
	for (uint32 x = 0; x < width * numColorComponents; x += numColorComponents)
	{
		const Color* topLeft = reinterpret_cast<const Color*>(&parentLevel->imageData[x + parentWidth * y]);
		const Color* topRight = reinterpret_cast<const Color*>(&parentLevel->imageData[(x + numColorComponents) + parentWidth * y]);
		const Color* botLeft = reinterpret_cast<const Color*>(&parentLevel->imageData[x + parentWidth * (y + 1)]);
		const Color* botRight = reinterpret_cast<const Color*>(&parentLevel->imageData[(x + numColorComponents) + parentWidth * (y + 1)]);

		int32 r = topLeft->r + topRight->r + botLeft->r + botRight->r;
		r /= 4;
		int32 g = topLeft->g + topRight->g + botLeft->g + botRight->g;
		g /= 4;
		int32 b = topLeft->b + topRight->b + botLeft->b + botRight->b;
		b /= 4;
		int32 a = topLeft->a + topRight->a + botLeft->a + botRight->a;
		a /= 4;

		currentLevel.imageData.Add(static_cast<uint8>(r));
		currentLevel.imageData.Add(static_cast<uint8>(g));
		currentLevel.imageData.Add(static_cast<uint8>(b));
		currentLevel.imageData.Add(static_cast<uint8>(a));
	}
}
//*/