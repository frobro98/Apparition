
#include "AnimationGameObject.h"
#include "Animation/Skeleton/SkeletonInstance.h"
#include "Animation/AnimationController.h"
#include "Input/Input.hpp"
#include "Model/Model.h"
#include "Archiver/SkeletonHeader.h"

void AnimationGameObject::SetupInputs()
{
// 	GetInputManager().RegisterStateChange(KeyInput::Key_Q, KeyStateFlags::KeyState_Pressed, this, &AnimationGameObject::SlowDownAnimation);
// 	GetInputManager().RegisterStateChange(KeyInput::Key_E, KeyStateFlags::KeyState_Pressed, this, &AnimationGameObject::SpeedUpAnimation);
// 	GetInputManager().RegisterStateChange(KeyInput::Key_Space, KeyStateFlags::KeyState_Pressed, this, &AnimationGameObject::TogglePause);
// 
// 	GetInputManager().RegisterStateChange(KeyInput::Key_F, KeyStateFlags::KeyState_Pressed, this, &AnimationGameObject::SwapCurrentAnimation);
// 	GetInputManager().RegisterStateChange(KeyInput::Key_B, KeyStateFlags::KeyState_Pressed, this, &AnimationGameObject::Blend);
}

void AnimationGameObject::SetSkeleton(Skeleton& skel)
{
	if (skeleton != nullptr)
	{
		delete skeleton;
	}
	skeleton = new SkeletonInstance(skel);
}

void AnimationGameObject::SetCurrentClip(u32 clipIndex)
{
	skeleton->SetCurrentAnimation(clipIndex);
}

void AnimationGameObject::Update(float tick)
{
	Time deltaTime = tick * Time(Time::ONE_MILLISECOND);

	//skeleton->SetWorld(world);
	skeleton->Update(deltaTime);

	//model->SetWorld(world);
	//model->SetActiveSkeleton(*skeleton);
	//model->SetBonePose(skeleton->GetPose());
}

void AnimationGameObject::SetDebug(bool isDebug)
{
	Assert(skeleton != nullptr);
	skeleton->SetDebug(isDebug);
}

void AnimationGameObject::TogglePause()
{
	if (playing)
	{
		playing = false;
		skeleton->GetController()->PauseAnimation();
	}
	else
	{
		playing = true;
		skeleton->GetController()->PlayAnimation();
	}
}

void AnimationGameObject::SpeedUpAnimation()
{
	skeleton->Faster();
}

void AnimationGameObject::SlowDownAnimation()
{
	skeleton->Slower();
}

void AnimationGameObject::SwapCurrentAnimation()
{
	skeleton->GoToNextClip();
}

void AnimationGameObject::Blend()
{
	skeleton->Blend();
}
