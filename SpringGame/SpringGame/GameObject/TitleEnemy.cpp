#include "TitleEnemy.h"
#include"../Game.h"
#include"../Physics/Matrix4x4.h"

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 200.0f,-120.0f,50.0f };

	//敵の生成位置
	const Vector3 kReSpawnPos = { 200.0f,-120.0f,0.0f };

	//移動速度
	constexpr float kEnemySpeed = 4.0f;

	//敵の向いている方向
	constexpr float kEnemyAngle = DX_PI_F * 1.5f;

	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//敵のモデルのパス
	const char* kEnemyPath = "data/Enemy.mv1";

	//モデルのサイズ
	constexpr float kModelScale = 0.5f;
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
	model_.Load(kEnemyPath);

	//アニメーション設定
	animation_.Init(model_.GetHandle(), AnimType::Enemy);
	animation_.ChangeState(AnimationState::Attack);

	pos_ = kFirstPos;
	angle_ = kEnemyAngle;

	time_ = 0.0f;
	speed_ = 0.0f;
}

void TitleEnemy::Update()
{
	float dt = kDeltaTime;

	speed_ += kEnemySpeed;

	pos_.x_ = pos_.x_ - kEnemySpeed;

	//アニメーションの更新
	animation_.Update(dt);
}

void TitleEnemy::Draw()
{
	Matrix4x4 mat =
		Matrix4x4::Scale(kModelScale, kModelScale, kModelScale) *
		Matrix4x4::RotateY(angle_) *
		Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	MV1SetMatrix(model_.GetHandle(), mat.ToDxLibMatrix());
	MV1DrawModel(model_.GetHandle());
}

void TitleEnemy::ReSpawn()
{
	pos_ = kReSpawnPos;
	speed_ = 0.0f;
}
