#include "Animation.h"
#include<Dxlib.h>
#include <cmath>

namespace
{
	const char* kAnimIdle = "Jump_Idle";
	const char* kAnimRun = "Running_A";
	const char* kAnimAttack = "Jump_Full";
	const char* kAnimDeath = "Walking_B";

	constexpr float kAnimationSpeed = 30.0f;
}

Animation::Animation() :
	modelHandle_(-1),
	currentAttach_(-1),
	currentAnim_(-1),
	currentTime_(0.0f),
	speed_(1.0f),
	isLoop_(true),
	totalTime_(0.0f),
	state_(AnimationState::Idle)
{

}

Animation::~Animation()
{
}

void Animation::Init(int modelHandle)
{
	modelHandle_ = modelHandle;
}

void Animation::Update(float deltaTime)
{
	if (currentAttach_ != -1 && totalTime_ == 0.0f)
	{
		totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttach_);
	}

	if (currentAttach_ != -1)
	{
		printf("totalTime = %f\n", totalTime_);
		currentTime_ += deltaTime * speed_;

		if (totalTime_ > 0.0f)
		{
			if (isLoop_)
			{
				if (currentTime_ > totalTime_)
				{
					currentTime_ = fmod(currentTime_, totalTime_);
				}
			}
			else
			{
				if (currentTime_ > totalTime_)
				{
					currentTime_ = totalTime_;
				}
			}
		}

		MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);
	}
}

void Animation::Play(int animIndex, float speed, bool isLoop)
{
	if (currentAnim_ == animIndex) return;

	speed_ = speed;
	isLoop_ = isLoop;

	if (currentAttach_ != -1)
	{
		MV1DetachAnim(modelHandle_, currentAttach_);
	}

	currentAnim_ = animIndex;
	currentTime_ = 0.0f;

	currentAttach_ = MV1AttachAnim(modelHandle_, currentAnim_);
	totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttach_);

	MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);
}

void Animation::ChangeState(AnimationState state)
{
	if (state_ == state) return;

	state_ = state;

	int animIndex = -1;

	switch (state_)
	{
	case AnimationState::Idle:
		animIndex = MV1GetAnimIndex(modelHandle_, kAnimIdle);
		break;

	case AnimationState::Run:
		animIndex = MV1GetAnimIndex(modelHandle_, kAnimRun);
		break;

	case AnimationState::Attack:
		animIndex = MV1GetAnimIndex(modelHandle_, kAnimAttack);
		break;

	case AnimationState::Death:
		animIndex = MV1GetAnimIndex(modelHandle_, kAnimDeath);
		break;
	}

	if (animIndex != -1)
	{
		bool loop = true;

		if (state_ == AnimationState::Attack || state_ == AnimationState::Death)
		{
			loop = false;
		}

		Play(animIndex, kAnimationSpeed, loop);
	}
}
