#include "Enemy.h"
#include "../Physics/Vector3.h"
#include "../Actor/Player.h"
#include<Dxlib.h>
#include<cassert>
#include<cmath>
#include<algorithm>

namespace
{
	//当たり判定のサイズ
	constexpr float kColSize = 50.0f;

	//初期位置
	const Vector3 kFirstPos = { 400.0f,0.0f,0.0f };

	//モデルのサイズ
	const VECTOR kModelScale = { 100.0f,100.0f, 100.0f };

	//追従開始距離
	constexpr float kChaseRange = 300.0f;

	//停止距離
	constexpr float kStopRange = 150.0f;

	//敵の速度
	constexpr float kSpeed = 4.0f;
}

Enemy::Enemy() :
	GameObject(Vector3(400.0f, 0.0f, 0.0f), Vector3(0, 0, 0)),
	modelHandle_(-1),
	collider_(kColSize),
	isHit_(false),
	hitTimer_(0.0f)
{
	pos_ = kFirstPos;
	collider_.SetOwner(this);
}

Enemy::~Enemy()
{
	MV1DeleteModel(modelHandle_);
}

void Enemy::Init()
{
	modelHandle_ = MV1LoadModel("data/Enemy.mv1");
	assert(modelHandle_ >= 0);
	MV1SetPosition(modelHandle_, pos_.ToDxlibVector());
	MV1SetScale(modelHandle_, kModelScale);

	collider_.SetEnable(true);
	collider_.SetColliderType(ColliderType::Charactor);
}

void Enemy::Update()
{
	if (!pPlayer_)return;

	//プレイヤーの向きのベクトル
	Vector3 toPlayer = pPlayer_->GetPos() - pos_;
	float distance = toPlayer.Length();

	//正規化
	Vector3 dir = { 0,0,0 };
	if (distance > 0.001f)
	{
		dir = toPlayer.Normalize();
	}

	//追従
	if (distance < kChaseRange)
	{
		if (distance > kStopRange)
		{
			//停止距離に近いほど減速する
			float t = (distance - kStopRange) / (kChaseRange - kStopRange);
			t = std::clamp(t, 0.0f, 1.0f);

			vel_ = dir * kSpeed * t;
		}
		else
		{
			//完全停止
			vel_ = Vector3(0, 0, 0);
		}
	}
	else
	{
		vel_ *= 0.9f;
	}

	pos_ += vel_;

	// タイマー減少
	if (hitTimer_ > 0.0f)
	{
		hitTimer_ -= 1.0f / 60.0f;
	}
	else
	{
		isHit_ = false;
	}

	collider_.SetPos(pos_ + Vector3(0.0f, 100.0f, 0.0f));
	MV1SetPosition(modelHandle_, pos_.ToDxlibVector());
	DrawFormatString(0, 32, GetColor(255, 255, 255),
		"Enemy: %.2f %.2f %.2f", pos_.x_, pos_.y_, pos_.z_);
}

void Enemy::Draw()
{
	MV1DrawModel(modelHandle_);

	// 当たり判定の描画
	unsigned int color = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(), // 中心
		collider_.GetRadian(),				// 半径
		16, color, color, FALSE);
}

void Enemy::OnHit(GameObject* attacker)
{
	isHit_ = true;
}

void Enemy::OnCollision(GameObject* other)
{
	isHit_ = true;
}
