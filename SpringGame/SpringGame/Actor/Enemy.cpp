#include "Enemy.h"
#include "../Scene/SceneMain.h"
#include "../ScoreManager.h"
#include "../Physics/Vector3.h"
#include "../Physics/Camera.h"
#include "../Actor/Player.h"
#include "../EffectManager.h"
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
	const VECTOR kModelScale = { 0.8f,0.8f, 0.8f };

	//追従開始距離
	constexpr float kChaseRange = 300.0f;

	//停止距離
	constexpr float kStopRange = 150.0f;

	//敵の速度
	constexpr float kSpeed = 4.0f;

	constexpr float kShakeTimeHit = 0.2f;
	constexpr float kShakePowerHit = 15.0f;
}

Enemy::Enemy() :
	GameObject(Vector3(400.0f, 0.0f, 0.0f), Vector3(0, 0, 0)),
	state_(AnimationState::Spawn),
	collider_(kColSize),
	isHit_(false),
	hitTimer_(0.0f),
	isDead_(false),
	isDestroy_(false),
	isSpawning_(true)
{
	pos_ = kFirstPos;
	collider_.SetOwner(this);

	//コライダーを登録
	GameObject::pCollider_ = &collider_;
}

Enemy::~Enemy()
{
	model_.Release();
}

void Enemy::Init()
{
	model_ .Load("data/Enemy.mv1");
	//copyModel_MV1DuplicateModel(model_.GetHandle());
	MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());
	MV1SetScale(model_.GetHandle(), kModelScale);

	animation_.Init(model_.GetHandle(), AnimType::Enemy);
	animation_.ChangeState(AnimationState::Spawn);

	collider_.SetEnable(false);
	collider_.SetColliderType(ColliderType::Charactor);
	collider_.SetPos(pos_ + Vector3(0.0f, 80.0f, 0.0f));
}

void Enemy::Update(float dt)
{
	if (!pPlayer_)return;

	//生成時の処理
	if (isSpawning_)
	{
		animation_.Update(dt);

		//終わったら通常状態へ
		if (animation_.IsEnd())
		{
			isSpawning_ = false;
			animation_.ChangeState(AnimationState::Idle);

			collider_.SetEnable(true);
		}

		//当たり判定も更新
		collider_.SetPos(pos_ + Vector3(0.0f, 80.0f, 0.0f));

		MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());
		return;
	}

	//死亡処理
	if (isDead_)
	{
		vel_ = Vector3(0, 0, 0);

		animation_.Update(dt);

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

	pos_ += vel_ * dt * 60.0f;

	//敵の向きを追従している際にプレイヤー方向に合わせる
	if (vel_.Length() > 0.001f)
	{
		float angleY = atan2f(-dir.x_, -dir.z_);
		MV1SetRotationXYZ(model_.GetHandle(), VGet(0.0f, angleY, 0.0f));
	}

	state_ = GetState();
	animation_.ChangeState(state_);
	animation_.Update(dt);

	//タイマー減少
	if (hitTimer_ > 0.0f)
	{
		hitTimer_ -= dt;
	}
	else
	{
		isHit_ = false;
	}

	collider_.SetPos(pos_ + Vector3(0.0f, 80.0f, 0.0f));
	MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());
}

void Enemy::Draw()
{
	model_.Draw();
#ifdef _DEBUG
	// 当たり判定の描画
	unsigned int color = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(), // 中心
		collider_.GetRadian(),				// 半径
		16, color, color, FALSE);
#endif
}

void Enemy::OnHit(GameObject* attacker)
{
	if (isDead_) return;

	isHit_ = true;
	isDead_ = true;

	//当たり判定を無効にする
	collider_.SetEnable(false);

	//点数加算
	int addScore = ScoreManager::AddScore();

	if (pScene_)
	{
		pScene_->AddScorePop(pos_ + Vector3(0, 200.0f, 0), addScore);
	}

	if (pCamera_)
	{
		pCamera_->Shake(kShakeTimeHit, kShakePowerHit);
	}

	EffectManager::GetInstance().Play("hit",pos_ + Vector3(0.0f, 80.0f, 0.0f));

	//死亡アニメーション開始
	animation_.ChangeState(AnimationState::Death);
}

void Enemy::OnCollision(GameObject* other)
{
	if (isDead_ || isSpawning_) return;

	isHit_ = true;

	if (other->GetCollider()->GetColliderType() == ColliderType::Attack)
	{
		other->GetCollider()->SetEnable(false);
		OnHit(other);
	}
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
