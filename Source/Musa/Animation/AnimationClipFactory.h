#pragma once

#include "Containers/DynamicArray.hpp"

class AnimationClip;

class AnimationClipFactory
{
public:

	static AnimationClip* CreateAnimationClip(u8* animationData);
	static void DestroyAnimationClip(AnimationClip* clip);

private:
	AnimationClipFactory() = default;
	~AnimationClipFactory();

	static AnimationClipFactory& Instance();

private:
	static AnimationClipFactory* cfInstance;

	DynamicArray<AnimationClip> clips;
};