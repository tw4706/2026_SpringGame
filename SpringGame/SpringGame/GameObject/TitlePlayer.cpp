#include "TitlePlayer.h"
#include "../Game.h"
#include"../Physics/Matrix4x4.h"

namespace
{
	constexpr float kHalfW = 500.0f;
	constexpr float kHalfH = 500.0f;
}

TitlePlayer::TitlePlayer() :
	GameObject(pos_, vel_),
	time_(0.0f),
	angle_(0.0f),
	isAttacking_(false)
{
}

TitlePlayer::~TitlePlayer()
{
	//モデルの解放
	model_.Release();
}

void TitlePlayer::Init()
{
	model_.Load("data/Player_Idle.mv1");

	//アニメーションの設定
	animation_.Init(model_.GetHandle(), AnimType::Player);
	animation_.ChangeState(AnimationState::Run);

	pos_ = { -200.0f, -120.0f, 0.0f };

	angle_ = DX_PI_F * 0.5f;

	time_ = 0.0f;
}

void TitlePlayer::Update()
{
	float dt = 1.0f / 60.0f;

	//アニメーションの更新
	animation_.Update(dt);

	//アニメーションが終わったら
	if (animation_.IsEnd())
	{
		//攻撃のフラグを戻す
		isAttacking_ = false;

		//走りアニメーションに戻す
		animation_.ChangeState(AnimationState::Run);
	}
}

void TitlePlayer::Draw()
{
	Matrix4x4 mat =
		Matrix4x4::Scale(1.0f, 1.0f, 1.0f) *
		Matrix4x4::RotateY(angle_) *
		Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	MV1SetMatrix(model_.GetHandle(), mat.ToDxLibMatrix());
	MV1DrawModel(model_.GetHandle());
}

void TitlePlayer::Attack()
{
	if (isAttacking_) return;

	isAttacking_ = true;
	animation_.ChangeState(AnimationState::Attack);
}
