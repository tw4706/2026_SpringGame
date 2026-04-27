#include "Enemy.h"
#include "../Physics/Vector3.h"
#include "../Physics/Camera.h"
#include "../GameObject/Player.h"
#include "../Manager/EffectManager.h"
#include"../System/Application.h"
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

	//カメラを揺らす時間
	constexpr float kCameraShakeTime = 0.2f;

	//カメラを揺らす力
	constexpr float kCameraShakePower = 15.0f;

	//当たり判定の位置調整
	const Vector3 kColOffset = { 0.0f,80.0f,0.0f };

	//移動速度の減速
	constexpr float kVelDeceration = 0.9f;

	//エフェクトの位置調整
	const Vector3 kEffectOffset = { 0.0f,80.0f,0.0f };

	//プレイヤーの向きのベクトルの正規化用
	constexpr float kDirectionEpsilon = 0.001f;

	//フレームレート
	constexpr float kFrameRate = 60.0f;

	//攻撃を開始する距離
	constexpr float kAttackRange = 120.0f;

	//攻撃のクールタイム
	constexpr float kAttackCooldown = 2.0f;

	//攻撃持続時間
	constexpr float kAttackDuration = 0.3f;

	//攻撃判定のサイズ
	constexpr float kAttackColSize = 80.0f;

	//予備動作時間
	constexpr float kPreAttackTime = 1.0f;
}

Enemy::Enemy() :
	GameObject(Vector3(400.0f, 0.0f, 0.0f), Vector3(0, 0, 0)),
	state_(AnimationState::Spawn),
	collider_(kColSize),
	attackCollider_(kAttackColSize),
	isHit_(false),
	hitTimer_(0.0f),
	isDead_(false),
	isDestroy_(false),
	isSpawning_(true),
	attackTimer_(0.0f),
	isAttacking_(false),
	isPrevAttacking_(false),
	prevAttackTimer_(0.0f)
{
	pos_ = kFirstPos;
	collider_.SetOwner(this);
	attackCollider_.SetOwner(this);

	//攻撃判定の初期化
	attackCollider_.SetEnable(false);
	attackCollider_.SetColliderType(ColliderType::Attack);
	attackCollider_.SetOwner(this);

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
	//モデルのポジションとサイズの設定
	model_.SetPosition(pos_);
	model_.SetScale(kModelScale);

	//アニメーションの初期化
	animation_.Init(model_.GetHandle(), AnimType::Enemy);
	animation_.ChangeState(AnimationState::Spawn);

	//当たり判定の初期化
	collider_.SetEnable(false);
	collider_.SetColliderType(ColliderType::Charactor);
	collider_.SetPos(pos_ + kColOffset);

	EffectManager::GetInstance().Play("spawn", pos_);
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
		collider_.SetPos(pos_ + kColOffset);

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
	if (distance > kDirectionEpsilon)
	{
		dir = toPlayer.Normalize();
	}

	//追従
	if (distance < kChaseRange)
	{
		if (distance < kAttackRange)
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

			if (attackTimer_ <= 0.0f && !isAttacking_ && !isPrevAttacking_)
			{
				isPrevAttacking_ = true;
				prevAttackTimer_ = kPreAttackTime;

				//攻撃準備エフェクト
				EffectManager::GetInstance().Play("enemyAttack", pos_);
			}
		}
	}
	else
	{
		vel_ *= kVelDeceration;
	}

	pos_ += vel_ * dt * kFrameRate;

	//敵の向きを追従している際にプレイヤー方向に合わせる
	if (vel_.Length() > 0.001f)
	{
		float angleY = atan2f(-dir.x_, -dir.z_);
		MV1SetRotationXYZ(model_.GetHandle(), VGet(0.0f, angleY, 0.0f));
	}

	//アニメーションの切り替え
	if (isAttacking_)
	{
		if (state_ != AnimationState::Attack)
		{
			animation_.ChangeState(AnimationState::Attack);
			state_ = AnimationState::Attack;
		}
	}
	else if (isPrevAttacking_)
	{
		animation_.ChangeState(AnimationState::PrevAttack);
	}
	else
	{
		state_ = GetState();
		animation_.ChangeState(state_);
	}
	animation_.Update(dt);

	//ヒットタイマー減少
	if (hitTimer_ > 0.0f)
	{
		hitTimer_ -= dt;
	}
	else
	{
		isHit_ = false;
	}

	//予備動作中
	if (isPrevAttacking_)
	{
		prevAttackTimer_ -= dt;

		if (prevAttackTimer_ <= 0.0f)
		{
			isPrevAttacking_ = false;

			//攻撃を開始させる
			isAttacking_ = true;
			attackTimer_ = kAttackDuration;
		}
	}

	if (isAttacking_)
	{
		attackTimer_ -= dt;

		//攻撃判定あり
		attackCollider_.SetEnable(true);

		Vector3 attackPos = pos_;
		attackCollider_.SetPos(attackPos);

		if (attackTimer_ <= 0.0f)
		{
			isAttacking_ = false;
			attackTimer_ = kAttackCooldown;

			attackCollider_.SetEnable(false);
		}
	}
	else
	{
		attackTimer_ -= dt;

		attackCollider_.SetEnable(false);
	}

	collider_.SetPos(pos_ + kColOffset);
	MV1SetPosition(model_.GetHandle(), pos_.ToDxlibVector());
}

void Enemy::Draw()
{
	model_.Draw();
#ifdef _DEBUG
	unsigned int color = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

	//敵の当たり判定描画
	DrawSphere3D(collider_.GetPos().ToDxlibVector(),
		collider_.GetRadian(),16, color, color, FALSE);

	//追従範囲の描画
	DrawSphere3D(pos_.ToDxlibVector(),kChaseRange,
		32,GetColor(0, 0, 255),GetColor(0, 0, 255),FALSE);

	//停止範囲の描画
	DrawSphere3D(pos_.ToDxlibVector(),kStopRange,
		32,GetColor(255, 255, 0),GetColor(255, 255, 0),FALSE);

	//攻撃範囲
	if (attackCollider_.IsEnable())
	{
		DrawSphere3D(
			attackCollider_.GetPos().ToDxlibVector(),
			attackCollider_.GetRadian(),
			16,
			GetColor(255, 0, 255),
			GetColor(255, 0, 255),
			FALSE);
	}
#endif
}

void Enemy::OnHit(GameObject* attacker)
{
	if (isDead_) return;

	isHit_ = true;
	isDead_ = true;

	//当たり判定を無効にする
	collider_.SetEnable(false);

	if (pCamera_)
	{
		pCamera_->Shake(kCameraShakeTime, kCameraShakePower);
	}

	//エフェクトの再生
	EffectManager::GetInstance().Play("hit",pos_ + kEffectOffset);

	//SE再生
	Application::GetInstance().GetSoundManager().PlaySe(SE::Attack);

	//死亡アニメーション開始
	animation_.ChangeState(AnimationState::Death);
}

void Enemy::OnCollision(GameObject* other)
{
	if (isDead_ || isSpawning_) return;

	//敵同士の攻撃判定は無効
	if (dynamic_cast<Enemy*>(other))
	{
		return;
	}

	isHit_ = true;

	if (other->GetCollider()->GetColliderType() == ColliderType::Charactor)
	{
		if (attackCollider_.IsEnable())
		{
			//ここでプレイヤーにダメージ
			other->OnHit(this);
		}
	}

	if (other->GetCollider()->GetColliderType() == ColliderType::Attack)
	{
		other->GetCollider()->SetEnable(false);
		OnHit(other);
	}
}

void Enemy::SetSpawnPos(const Vector3& spawnPos)
{
	pos_ = spawnPos;
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
