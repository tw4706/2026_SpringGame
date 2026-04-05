#include "Player.h"
#include"../Input.h"
#include"../Physics/Matrix4x4.h"
#include"../Physics/Vector3.h"
#include "../Actor/Enemy.h"
#include"../EffectManager.h"
#include"../Physics/Camera.h"
#include"DxLib.h"
#include<cassert>
#include<algorithm>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f,0.0f,0.0f };

	constexpr int kMaxHP = 3;

	//プレイヤーの移動速度
	constexpr float kSpeed = 16.0f;

	constexpr float kCameraSpeed = 0.05f;

	//当たり判定のサイズ
	constexpr float kColSize = 50.0f;
	constexpr float kAttackColSize = 80.0f;

	//当たり判定位置の調整
	const Vector3 kColOffset = { 0.0f,80.0f,0.0f };

	//モデルのサイズ
	constexpr float kModelScale = 0.5f;

	//移動制限するための定数
	const float kWalkLimit = 950.0f;

	constexpr float kDodgeTime = 0.25f;
	constexpr float kDodgeSpeed = 18.0f;
	constexpr int kJustDodgeFrame = 3;
}

Player::Player() :
	GameObject(pos_, vel_),
	state_(PlayerState::Idle),
	cameraAngle_(0.0f),
	moveAngle_(0.0f),
	attackTimer_(0.0f),
	invincibleTimer_(0.0f),
	dodgeTimer_(0.0f),
	hitTimer_(0.0f),
	afterImageTimer_(0.0f),
	isJustDodge_(false),
	isJustDodgeTriggered_(false),
	justDodgeFrame_(0),
	hp_(kMaxHP),
	collider_(0.0f),
	attackCollider_(0.0f),
	isHit_(false),
	knockbackVel_(0.0f, 0.0f, 0.0f),
	knockbackTimer_(0.0f)
{
	//初期位置
	pos_ = kFirstPos;

	//当たり判定(キャラクター・攻撃)
	collider_ = SphereCollider(kColSize);
	attackCollider_ = SphereCollider(kAttackColSize);

	//当たり判定の所有者
	collider_.SetOwner(this);
	attackCollider_.SetOwner(this);

	//コライダーを登録
	GameObject::pCollider_ = &collider_;
}

Player::~Player()
{
	model_.Release();
}

void Player::Init()
{
	//モデルのロード
	model_.Load("data/Player.mv1");
	ghostModel_.Load("data/PlayerAfterImage.mv1");

	//アニメーションの初期化
	animation_.Init(model_.GetHandle(), AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);

	//当たり判定の設定
	collider_.SetEnable(true);
	attackCollider_.SetEnable(false);

	collider_.SetColliderType(ColliderType::Charactor);
	attackCollider_.SetColliderType(ColliderType::Attack);
}

void Player::Update(Input& input, float dt)
{
	isHit_ = false;

	//タイマーの更新
	UpdateTimers(dt);
	//入力処理
	HandleInput(input);

	//状態別の処理
	UpdateAction(input, dt);

	//ノックバックの更新
	UpdateKnockBack(dt);

	//状態遷移の処理
	UpdateState();

	//アニメーションの更新
	UpdateAnimation(dt);

	//当たり判定の更新
	UpdateCollision();

	//移動の制限 
	if (pos_.x_ > kWalkLimit) pos_.x_ = kWalkLimit;
	if (pos_.x_ < -kWalkLimit) pos_.x_ = -kWalkLimit;
	if (pos_.z_ > kWalkLimit) pos_.z_ = kWalkLimit;
	if (pos_.z_ < -kWalkLimit) pos_.z_ = -kWalkLimit;

	//行列の更新 
	UpdateMatrix();

	for (auto it = afterImages_.begin(); it != afterImages_.end(); )
	{
		it->life -= dt;

		if (it->life <= 0.0f)
		{
			MV1DeleteModel(it->modelHandle);
			it = afterImages_.erase(it);
		}
		else
		{
			++it;
		}
	}

	afterImages_.erase(
		std::remove_if(afterImages_.begin(), afterImages_.end(),
			[](const AfterImage& a) { return a.life <= 0.0f; }),
		afterImages_.end()
	);
}

void Player::Draw()
{

	for (auto& img : afterImages_)
	{
		Matrix4x4 mat =
			Matrix4x4::Scale(kModelScale, kModelScale, kModelScale) *
			Matrix4x4::RotateY(img.angle + 3.141592f) *
			Matrix4x4::Translate(img.pos.x_, img.pos.y_, img.pos.z_);

		MV1SetMatrix(img.modelHandle, mat.ToDxLibMatrix());
		MV1DrawModel(img.modelHandle);
	}

	//モデルの描画
	model_.Draw();

#ifdef _DEBUG
	//当たり判定の描画
	unsigned int color = isHit_ ? 0xff0000 : 0x00ff00;

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(),		//中心
		collider_.GetRadian(),					//半径
		16, color, color, FALSE);

	//攻撃判定の描画
	if (state_ == PlayerState::Attack)
	{
		DrawSphere3D(
			attackCollider_.GetPos().ToDxlibVector(),
			attackCollider_.GetRadian(),
			16,
			0xffff00,
			0xffff00,
			FALSE);
	}
#endif
}

void Player::Move(Input& input, float dt)
{
	//移動ベクトルの初期化
	vel_ = { 0.0f, 0.0f, 0.0f };

	//キーボード入力から方向作る 
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
	if (input.IsPressed("up")) inputDir.z_ += 1.0f;
	if (input.IsPressed("down")) inputDir.z_ -= 1.0f;
	if (input.IsPressed("left")) inputDir.x_ -= 1.0f;
	if (input.IsPressed("right")) inputDir.x_ += 1.0f;

	//入力があるときだけ 
	if (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.01f)
	{
		//カメラの角度を基準にして移動方向を回転させる
		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraAngle_);
		Vector3 playerDir = rotMat.TransformForVector(inputDir);

		//正規化 
		playerDir.Normalize();

		//移動 
		vel_.x_ = playerDir.x_ * kSpeed;
		vel_.z_ = playerDir.z_ * kSpeed;

		//向き更新 
		float playerAngle = atan2f(-playerDir.x_, playerDir.z_);
		float diff = playerAngle - moveAngle_;

		//キャラクターの角度を補間する
		while (diff > 3.141592f) diff -= 6.28318f;
		while (diff < -3.141592f) diff += 6.28318f;

		//スムーズに回転させる
		moveAngle_ += diff * 0.2f;
	}

	//アナログスティックの更新 
	UpdateAnalogStick(input);

	//位置の反映
	pos_ += vel_ * dt * 60.0f;
}

void Player::StartAttack()
{
	state_ = PlayerState::Attack;
	attackTimer_ = 0.3f;
}

void Player::StartDodge()
{
	state_ = PlayerState::Dodge;
	dodgeTimer_ = kDodgeTime;

	//ジャスト回避の判定開始
	justDodgeFrame_ = kJustDodgeFrame;
	isJustDodge_ = false;
}

void Player::UpdateTimers(float deltaTime)
{

	if (attackTimer_ > 0.0f) attackTimer_ -= deltaTime;
	if (dodgeTimer_ > 0.0f) dodgeTimer_ -= deltaTime;
	if (hitTimer_ > 0.0f)hitTimer_ -= deltaTime;
	if (invincibleTimer_ > 0.0f) invincibleTimer_ -= deltaTime;
	if (justDodgeFrame_ > 0)justDodgeFrame_--;
}

void Player::HandleInput(Input& input)
{
	if (knockbackTimer_ > 0.0f) return;

	//被ダメージ中や死亡中は入力を受け付けない
	if (state_ == PlayerState::Hit || state_ == PlayerState::Death)
	{
		return;
	}

	if (input.IsTriggered("dodge") && state_ != PlayerState::Dodge)
	{
		StartDodge();
		return;
	}

	if (input.IsTriggered("attack") && state_ != PlayerState::Attack)
	{
		StartAttack();
		return;
	}
}

void Player::UpdateAction(Input& input, float dt)
{
	switch (state_)
	{
	case PlayerState::Idle:
	case PlayerState::Run:
		//ノックバック中は移動処理を行わない
		if (knockbackTimer_ <= 0.0f)
		{
			Move(input, dt);
		}
		break;

	case PlayerState::Attack:
		UpdateAttack();
		break;

	case PlayerState::Dodge:
		UpdateDodge(dt);
		break;
	case PlayerState::Hit:
		break;
	case PlayerState::Death:
		break;
	}
}

void Player::UpdateState()
{
	if (state_ == PlayerState::Attack && attackTimer_ <= 0.0f)
		state_ = PlayerState::Idle;

	if (state_ == PlayerState::Dodge && dodgeTimer_ <= 0.0f)
		state_ = PlayerState::Idle;

	if (state_ == PlayerState::Idle || state_ == PlayerState::Run)
	{
		if (fabs(vel_.x_) > 0.1f || fabs(vel_.z_) > 0.1f)
			state_ = PlayerState::Run;
		else
			state_ = PlayerState::Idle;
	}
	//ダメージ処理から戻す
	if (state_ == PlayerState::Hit && hitTimer_ <= 0.0f)
	{
		state_ = PlayerState::Idle;
	}

	//死亡は戻らない
	if (state_ == PlayerState::Death)
	{
		return;
	}
}

void Player::UpdateAttack()
{
	//プレイヤーの前方に攻撃判定を生成する
	Vector3 forward = { -sinf(moveAngle_), 0.0f, cosf(moveAngle_) };
	Vector3 attackPos = pos_ + forward * 100.0f + kColOffset;

	attackCollider_.SetPos(attackPos);

	if (attackTimer_ > 0.25f)
	{
		attackCollider_.SetEnable(true);
	}
	else
	{
		attackCollider_.SetEnable(false);
	}

	if (attackTimer_ <= 0.0f)
	{
		attackCollider_.SetEnable(false);
	}
}

void Player::UpdateKnockBack(float dt)
{
	if (knockbackTimer_ > 0.0f)
	{
		pos_ += knockbackVel_ * dt * 3.0f;

		// 減速（自然に止まる）
		knockbackVel_ *= 0.9f;

		knockbackTimer_ -= dt;
	}
}

void Player::UpdateDodge(float dt)
{
	//前方向に高速移動する
	Vector3 dir = { -sinf(moveAngle_), 0.0f, cosf(moveAngle_) };

	pos_ += dir * kDodgeSpeed * dt * 60.0f;

	//回避中は当たり判定を無効
	collider_.SetPos(pos_ + kColOffset);

	//無敵終了後に当たり判定を有効にする
	if (dodgeTimer_ <= 0.0f)
	{
		collider_.SetEnable(true);
		collider_.SetPos(pos_ + kColOffset);
	}

	afterImageTimer_ -= dt;

	if (afterImageTimer_ <= 0.0f)
	{
		afterImages_.push_back({ MV1DuplicateModel(ghostModel_.GetHandle()), pos_, moveAngle_, 0.3f });
		afterImageTimer_ = 0.02f; //速さ
	}
}

void Player::UpdateAnimation(float dt)
{
	AnimationState animState = AnimationState::Idle;

	switch (state_)
	{
	case PlayerState::Idle:		animState = AnimationState::Idle; break;
	case PlayerState::Run:		animState = AnimationState::Run; break;
	case PlayerState::Attack:	animState = AnimationState::Attack; break;
	case PlayerState::Hit:		animState = AnimationState::Hit; break;
	case PlayerState::Dodge:	animState = AnimationState::Dodge; break;
	case PlayerState::Death:	animState = AnimationState::Death; break;
	}

	animation_.ChangeState(animState);
	animation_.Update(dt);
}

void Player::UpdateCollision()
{
	if (state_ != PlayerState::Attack)
	{
		attackCollider_.SetEnable(false);
	}

	collider_.SetPos(pos_ + kColOffset);
}

void Player::UpdateAnalogStick(Input& input)
{
	//左アナログスティックの取得
	Vector3 stickL = input.GetStickLeft();
	Vector3 stickR = input.GetStickRight();

	//左スティックでプレイヤー操作
	if (fabs(stickL.x_) > 0.2f || fabs(stickL.z_) > 0.2f)
	{
		Matrix4x4 rotMat = Matrix4x4::RotateY(cameraAngle_);

		Vector3 playerDir = rotMat.TransformForVector(stickL);

		//移動
		vel_.x_ = playerDir.x_ * kSpeed;
		vel_.z_ = playerDir.z_ * kSpeed;

		//プレイヤーの向き
		float playerAngle = atan2f(-playerDir.x_, playerDir.z_);

		float diff = playerAngle - moveAngle_;

		//角度の上限と下限を設定する(diffが180度以上の間は角度を
		// 360引くことで違和感なく回転して見える。マイナスは逆のことをしているだけ)
		while (diff > 3.141592f) diff -= 6.28318f;
		while (diff < -3.141592f) diff += 6.28318f;

		// 補間
		moveAngle_ += diff * 0.2f;
	}

	//右スティックでカメラ操作
	if (fabs(stickR.x_) > 0.1f)
	{
		cameraAngle_ += stickR.x_ * kCameraSpeed;
	}
}

void Player::UpdateMatrix()
{
	//回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(moveAngle_ + 3.141592f);

	//移動
	Matrix4x4 transMat = Matrix4x4::Translate(pos_.x_, pos_.y_, pos_.z_);

	//拡縮
	Matrix4x4 scaleMat = Matrix4x4::Scale(kModelScale, kModelScale, kModelScale);

	//行列の合成
	Matrix4x4 mat = scaleMat * rotMat * transMat;
	MV1SetMatrix(model_.GetHandle(), mat.ToDxLibMatrix());
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}

void Player::OnCollision(GameObject* other)
{
	if (dynamic_cast<Enemy*>(other))
	{
		//ジャスト回避判定
		if (state_ == PlayerState::Dodge && justDodgeFrame_ > 0)
		{
			if (!isJustDodge_)
			{
				EffectManager::GetInstance().Play("dodge", pos_);
				isJustDodge_ = true;
				isJustDodgeTriggered_ = true;

				invincibleTimer_ = 0.5f;
			}
			return;
		}

		OnHit(other);
	}
}

bool Player::ConsumeJustDodge()
{
	if (isJustDodgeTriggered_)
	{
		isJustDodgeTriggered_ = false;
		return true;
	}
	return false;
}

void Player::OnHit(GameObject* attacker)
{
	if (IsInvincible()) return;

	hp_--;

	isHit_ = true;

	Vector3 dir = pos_ - attacker->GetPos();
	dir.y_ = 0.0f;
	dir.Normalize();

	moveAngle_ = atan2f(-dir.x_, dir.z_);
	knockbackVel_ = dir * 5.0f;
	knockbackTimer_ = 0.2f;

	if (hp_ <= 0)
	{
		hp_ = 0;
		state_ = PlayerState::Death;
	}
	else
	{
		state_ = PlayerState::Hit;
		hitTimer_ = 0.3f;
	}

	//カメラの揺れ
	if (pCamera_)
	{
		pCamera_->Shake(0.2f, 10.0f);
	}

	//無敵時間
	invincibleTimer_ = 0.5f;
}
