#include "Animation.h"
#include<Dxlib.h>

namespace
{
    const char* kAnimIdle = "Idle";
    const char* kAnimRun = "Run";
    const char* kAnimAttack = "Attack";
    const char* kAnimDeath = "Death";
}

Animation::Animation() :
	modelHandle_(-1),
	currentAttach_(-1),
	currentAnim_(-1),
	currentTime_(0.0f),
	nextAttach_(-1),
	nextAnim_(-1),
	nextTime_(0.0f),
	speed_(1.0f),
	isLoop_(true),
	blendRate_(0.0f),
	blendSpeed_(5.0f),
	isBlending_(false),
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
    // 現在アニメ
    if (currentAttach_ != -1)
    {
        currentTime_ += deltaTime * speed_;
        MV1SetAttachAnimTime(modelHandle_, currentAttach_, currentTime_);
    }

    // 次アニメ
    if (nextAttach_ != -1)
    {
        nextTime_ += deltaTime * speed_;
        MV1SetAttachAnimTime(modelHandle_, nextAttach_, nextTime_);
    }

    // ブレンド処理
    if (isBlending_)
    {
        blendRate_ += deltaTime * blendSpeed_;

        if (blendRate_ >= 1.0f)
        {
            blendRate_ = 1.0f;
            isBlending_ = false;

            if (currentAttach_ != -1)
            {
                MV1DetachAnim(modelHandle_, currentAttach_);
            }

            currentAttach_ = nextAttach_;
            currentAnim_ = nextAnim_;
            currentTime_ = nextTime_;

            nextAttach_ = -1;
        }
    }

    // ブレンド比率適用
    if (currentAttach_ != -1)
    {
        MV1SetAttachAnimBlendRate(modelHandle_, currentAttach_, 1.0f - blendRate_);
    }

    if (nextAttach_ != -1)
    {
        MV1SetAttachAnimBlendRate(modelHandle_, nextAttach_, blendRate_);
    }
}

void Animation::Play(int animIndex, float speed, bool isLoop)
{
	if (currentAnim_ == animIndex) return;

	speed_ = speed;
	isLoop_ = isLoop;

	// 次アニメとしてセット
	nextAnim_ = animIndex;
	nextTime_ = 0.0f;

	nextAttach_ = MV1AttachAnim(modelHandle_, nextAnim_);
	MV1SetAttachAnimTime(modelHandle_, nextAttach_, nextTime_);

	// ブレンド開始
	blendRate_ = 0.0f;
	isBlending_ = true;
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

        Play(animIndex, 1.0f, loop);
    }
}
