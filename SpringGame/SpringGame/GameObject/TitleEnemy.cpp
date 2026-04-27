#include "TitleEnemy.h"
#include"../Game.h"
#include"../Physics/Matrix4x4.h"

namespace
{
	constexpr float kEnemySpeed = 4.0f;
}

TitleEnemy::TitleEnemy():
	GameObject(pos_,vel_),
	time_(0.0f),
	speed_(0.0f),
	angle_(0.0f)
{
}

TitleEnemy::~TitleEnemy()
{
	model_.Release();
}

void TitleEnemy::Init()
{
	//モデルのロード
	model_.Load("data/Enemy.mv1");

	//アニメーション設定
	animation_.Init(model_.GetHandle(), AnimType::Enemy);
	animation_.ChangeState(AnimationState::Attack);

	pos_ = { 200.0f,-120.0f,50.0f };
	angle_ = DX_PI_F * 1.5f;

	time_ = 0.0f;
	speed_ = 0.0f;
}

void TitleEnemy::Update()
{
	float dt = 1.0f / 60.0f;

	speed_ += kEnemySpeed;

	pos_.x_ = pos_.x_ - kEnemySpeed;

	//アニメーションの更新
	animation_.Update(dt);
}

void TitleEnemy::Draw()
{
	Matrix4x4 mat =
		Matrix4x4::Scale(0.5f, 0.5f, 0.5f) *
		Matrix4x4::RotateY(angle_) *
		Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	MV1SetMatrix(model_.GetHandle(), mat.ToDxLibMatrix());
	MV1DrawModel(model_.GetHandle());
}

void TitleEnemy::ReSpawn()
{
	pos_ = { 600.0f,-120.0f,0.0f };
	speed_ = 0.0f;
}
