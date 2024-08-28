
#include "AnimationMixer.h"
#include "KeyFrame.h"
#include "Math/VectorFunctions.hpp"
#include "Math/QuatFunctions.hpp"

AnimationMixer::AnimationMixer(u32 skelBoneCount)
	: resultingFrame(new KeyFrame(skelBoneCount))
{
}

AnimationMixer::~AnimationMixer()
{
	delete resultingFrame;
}

void AnimationMixer::InterpolateAnimationFrames(Time deltaTime, const KeyFrame* frameA, const KeyFrame* frameB)
{
	resultingFrame->frameTime = deltaTime;
	float deltaS = (deltaTime - frameA->frameTime) / (frameB->frameTime - frameA->frameTime);

	u32 totalBones = resultingFrame->boneCount;
	const FrameData* boneA = frameA->boneFrameData;
	const FrameData* boneB = frameB->boneFrameData;
	FrameData* resultingBone = resultingFrame->boneFrameData;

	for (u32 i = 0; i < totalBones; ++i)
	{
		resultingBone->translation = Math::Lerp(boneA->translation, boneB->translation, deltaS);
		resultingBone->rotation = Math::Slerp(boneA->rotation, boneB->rotation, deltaS);
		resultingBone->scale = Math::Lerp(boneA->scale, boneB->scale, deltaS);

		++boneA;
		++boneB;
		++resultingBone;
	}
}

KeyFrame* AnimationMixer::GetResultingFrame() const
{
	return resultingFrame;
}
