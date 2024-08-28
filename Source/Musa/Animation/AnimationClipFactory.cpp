
#include "AnimationClipFactory.h"
#include "Clip.h"
#include "Archiver/AnimationHeader.h"
#include "Debugging/DebugOutput.hpp"

AnimationClip* AnimationClipFactory::CreateAnimationClip(u8* animationData)
{
	AnimationHeader* header = reinterpret_cast<AnimationHeader*>(animationData);
	u32 skeletonHash = header->referenceSkeleton;
	u32 frameCount = header->keyFrameCount;
	KeyFrame* frames = new KeyFrame[frameCount];
	u8* animationFrame = animationData + sizeof(AnimationHeader);
	for (u32 i = 0; i < frameCount; ++i)
	{
		KeyFrame* animFrame = reinterpret_cast<KeyFrame*>(animationFrame);
		KeyFrame* currentFrame = &frames[i];
		currentFrame->boneCount = animFrame->boneCount;
		currentFrame->frameTime = animFrame->frameTime;
		currentFrame->boneFrameData = new FrameData[animFrame->boneCount];
		
		animationFrame = animationFrame + sizeof(KeyFrame);

		for (u32 j = 0; j < animFrame->boneCount; ++j)
		{
			FrameData* frameData = reinterpret_cast<FrameData*>(animationFrame);
			FrameData* currentBone = &currentFrame->boneFrameData[j];
			
			currentBone->translation = frameData->translation;
			currentBone->rotation = frameData->rotation;
			currentBone->scale = frameData->scale;

// 			Debug::Printf("Translation: x(%f) y(%f) z(%f)\nRotation: x(%f) y(%f) z(%f) w(%f)\nScale x(%f) y(%f) z(%f)\n\n",
// 				currentBone->translation[x], currentBone->translation[y], currentBone->translation[z],
// 				currentBone->rotation[x], currentBone->rotation[y], currentBone->rotation[z], currentBone->rotation[w],
// 				currentBone->scale[x], currentBone->scale[y], currentBone->scale[z]
// 			);

			size_t dataSize = sizeof(FrameData);
			animationFrame += dataSize;
		}
	}

	AnimationClip* clip = new AnimationClip(header->animationName, frames, frameCount, header->totalAnimationTime * Time(Time::ONE_MILLISECOND), skeletonHash, header->frameRate);

	return clip;
}

void AnimationClipFactory::DestroyAnimationClip(AnimationClip* clip)
{
	delete clip;
}

AnimationClipFactory::~AnimationClipFactory()
{
}

AnimationClipFactory& AnimationClipFactory::Instance()
{
	static AnimationClipFactory acFactory;
	return acFactory;
}
