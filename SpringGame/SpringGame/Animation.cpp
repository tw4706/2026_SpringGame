#include "Animation.h"
#include<Dxlib.h>
#include <cmath>

namespace
{
	//プレイヤー
	const char* kPlayerIdle = "Jump_Idle";
	const char* kPlayerRun = "Running_A";
	const char* kPlayerAttack = "Jump_Full";
	const char* kPlayerDeath = "Walking_B";

	//敵
	const char* kEnemySpawn = "Spawn_Ground";
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
	prevAttach_(-1),
	blendTime_(0.0f),
	blendDuration_(0.3f),
	isBlending_(false),
	speed_(1.0f),
	isLoop_(true),
	isAnimEnd_(false),
	totalTime_(0.0f),
	state_(AnimationState::Idle),
	prevState_(AnimationState::Idle)
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
					isAnimEnd_ = true;
				}
			}
		}

		MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);

		if (prevAttach_ != -1)
		{
			float prevTime = MV1GetAttachAnimTime(modelHandle_, prevAttach_);
			float prevTotal = MV1GetAttachAnimTotalTime(modelHandle_, prevAttach_);

			prevTime += deltaTime * speed_;

			if (prevTotal > 0.0f)
			{
				if (isLoop_)
				{
					if (prevTime > prevTotal)
					{
						prevTime = fmod(prevTime, prevTotal);
					}
				}
				else
				{
					if (prevTime > prevTotal)
					{
						prevTime = prevTotal;
					}
				}
			}

			MV1SetAttachAnimTime(modelHandle_, prevAttach_, prevTime);
		}
	}
	if (isBlending_)
	{
		blendTime_ += deltaTime;

		float t = 1.0f;

		if (blendDuration_ > 0.0f)
		{
			t = blendTime_ / blendDuration_;
		}

		if (t > 1.0f) t = 1.0f;

		// 新アニメ
		MV1SetAttachAnimBlendRate(modelHandle_, currentAttach_, t);

		// 旧アニメ
		if (prevAttach_ != -1)
		{
			MV1SetAttachAnimBlendRate(modelHandle_, prevAttach_, 1.0f - t);
		}

		// 完了
		if (t >= 1.0f)
		{
			if (prevAttach_ != -1)
			{
				MV1DetachAnim(modelHandle_, prevAttach_);
				prevAttach_ = -1;
			}
			isBlending_ = false;
		}
	}
}

void Animation::Play(int animIndex, float speed, bool isLoop)
{

	if (currentAnim_ == animIndex && currentAttach_ != -1) return;

	isAnimEnd_ = false;
	speed_ = speed;
	isLoop_ = isLoop;

	if (prevAttach_ != -1)
	{
		MV1DetachAnim(modelHandle_, prevAttach_);
		prevAttach_ = -1;
	}

	//現在再生しているアニメーションを保存(生成状態の時はブレンドさせない)
	if (prevState_ == AnimationState::Spawn)
	{
		if (currentAttach_ != -1)
		{
			MV1DetachAnim(modelHandle_, currentAttach_);
		}

		prevAttach_ = -1; //ブレンドなし
	}
	else
	{
		prevAttach_ = currentAttach_;
	}

	currentAnim_ = animIndex;
	currentTime_ = 0.0f;

	currentAttach_ = MV1AttachAnim(modelHandle_, currentAnim_);
	totalTime_ = MV1GetAttachAnimTotalTime(modelHandle_, currentAttach_);

	blendTime_ = 0.0f;
	isBlending_ = true;

	//ウェイト初期化
	MV1SetAttachAnimBlendRate(modelHandle_, currentAttach_, 0.0f);

	if (prevAttach_ != -1)
	{
		MV1SetAttachAnimBlendRate(modelHandle_, prevAttach_, 1.0f);
	}

	MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);
}

void Animation::ChangeState(AnimationState state)
{
	if (state_ == state && currentAttach_ != -1) return;

	prevState_=state_;
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
		case AnimationState::Spawn:
			animIndex = MV1GetAnimIndex(modelHandle_, kEnemySpawn);
			break;
		}
	}

	if (animIndex != -1)
	{
		bool loop = true;

		if (state_ == AnimationState::Attack ||
			state_ == AnimationState::Death||
			state_ == AnimationState::Spawn)
		{
			loop = false;
		}

		Play(animIndex, kAnimationSpeed, loop);
	}
}
