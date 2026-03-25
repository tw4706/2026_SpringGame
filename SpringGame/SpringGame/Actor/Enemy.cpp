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
	collider_(kColSize),
	isHit_(false),
	hitTimer_(0.0f),
	isDead_(false),
	isDestroy_(false)
{
	pos_ = kFirstPos;
	collider_.SetOwner(this);
}

Enemy::~Enemy()
{
	model_.Release();
}

void Enemy::Init()
{
	model_ .Load("data/Enemy.mv1");
	MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());
	MV1SetScale(model_.GetHandle(), kModelScale);

	animation_.Init(model_.GetHandle(), AnimType::Enemy);
	animation_.ChangeState(AnimationState::Idle);

	collider_.SetEnable(true);
	collider_.SetColliderType(ColliderType::Charactor);
}

void Enemy::Update()
{
	if (!pPlayer_)return;

	//死亡処理
	if (isDead_)
	{
		vel_ = Vector3(0, 0, 0);

		animation_.Update(1.0f / 60.0f);

		//アニメーションが終わったら削除
		if (animation_.IsEnd())
		{
			isDestroy_ = true;
		}

		//位置更新
		MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());

		return;
	}

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

	state_ = GetState();
	animation_.ChangeState(state_);
	animation_.Update(1.0f / 60.0f);

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
	MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());
#ifdef DEBUG_
	DrawFormatString(0, 32, GetColor(255, 255, 255),
		"Enemy: %.2f %.2f %.2f", pos_.x_, pos_.y_, pos_.z_);
#endif
}

void Enemy::Draw()
{
	model_.Draw();

	// 当たり判定の描画
	unsigned int color = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(), // 中心
		collider_.GetRadian(),				// 半径
		16, color, color, FALSE);
}

void Enemy::OnHit(GameObject* attacker)
{
	if (isDead_) return;

	isHit_ = true;
	isDead_ = true;

	//死亡アニメーション開始
	animation_.ChangeState(AnimationState::Death);
}

void Enemy::OnCollision(GameObject* other)
{
	isHit_ = true;
}

AnimationState Enemy::GetState()const
{
	float speed = vel_.Length();

	if (speed > 0.1f)
	{
		return AnimationState::Run;
	}

	return AnimationState::Idle;
}
