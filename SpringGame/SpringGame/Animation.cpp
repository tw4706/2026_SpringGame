#include "Animation.h"
#include<Dxlib.h>
#include <cmath>

namespace
{
	//ƒvƒŒƒCƒ„[
	const char* kPlayerIdle = "Jump_Idle";
	const char* kPlayerRun = "Running_A";
	const char* kPlayerAttack = "Jump_Full";
	const char* kPlayerDeath = "Walking_B";

	//“G
	const char* kEnemyIdle = "Idle_A";
	const char* kEnemyRun = "Idle_B";
	const char* kEnemyDeath = "Death_A";

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

void Animation::Init(int modelHandle,AnimType type)
{
	modelHandle_ = modelHandle;
	type_ = type;
}

void Animation::Update(float deltaTime)
{
	if (currentAttach_ != -1 && totalTime_ == 0.0f)
	{
		totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttach_);
	}

	if (currentAttach_ != -1)
	{
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
	if (state_ == state && currentAttach_ != -1) return;

	state_ = state;

	int animIndex = -1;

	if (type_ == AnimType::Player)
	{
		switch (state_)
		{
		case AnimationState::Idle:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerIdle);
			break;
		case AnimationState::Run:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerRun);
			break;
		case AnimationState::Attack:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerAttack);
			break;
		case AnimationState::Death:
			animIndex = MV1GetAnimIndex(modelHandle_, kPlayerDeath);
			break;
		}
	}
	else if (type_ == AnimType::Enemy)
	{
		switch (state_)
		{
		case AnimationState::Idle:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyIdle);
			break;
		case AnimationState::Run:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyRun);
			break;
		case AnimationState::Death:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemyDeath);
			break;
		}
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
