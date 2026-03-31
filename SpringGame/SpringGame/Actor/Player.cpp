#include "Player.h"
#include"../Input.h"
#include"../Physics/Matrix4x4.h"
#include"../Physics/Vector3.h"
#include "../Actor/Enemy.h"
#include"DxLib.h"
#include<cassert>

namespace
{
	//初期位置
	const Vector3 kFirstPos = { 0.0f,0.0f,0.0f };

	//プレイヤーの移動速度
	constexpr float kSpeed = 8.0f;

	constexpr float kCameraSpeed = 0.05f;

	//当たり判定のサイズ
	constexpr float kColSize = 50.0f;
	constexpr float kAttackColSize = 50.0f;

	//当たり判定位置の調整
	const Vector3 kColOffset = { 0.0f,80.0f,0.0f };

	//モデルのサイズ
	constexpr float kModelScale = 0.5f;

	//移動制限するための定数
	const float kWalkLimit = 950.0f;

	constexpr float kDodgeTime = 0.25f;
	constexpr float kDodgeSpeed = 18.0f;
}

Player::Player() :
	GameObject(pos_, vel_),
	state_(PlayerState::Idle),
	cameraAngle_(0.0f),
	moveAngle_(0.0f),
	attackTimer_(0.0f),
	invincibleTimer_(0.0f),
	dodgeTimer_(0.0f),
	collider_(0.0f),
	attackCollider_(0.0f),
	isHit_(false)
{
	pos_ = kFirstPos;
	collider_ = SphereCollider(kColSize);
	attackCollider_ = SphereCollider(kAttackColSize);
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
	model_.Load("data/Player.mv1");

	animation_.Init(model_.GetHandle(), AnimType::Player);
	animation_.ChangeState(AnimationState::Idle);

	collider_.SetEnable(true);
	attackCollider_.SetEnable(false);

	collider_.SetColliderType(ColliderType::Charactor);
	attackCollider_.SetColliderType(ColliderType::Attack);
}

void Player::Update(Input& input)
{
	isHit_ = false;

	UpdateTimers();
	HandleInput(input);
	UpdateAction(input);
	UpdateState();
	UpdateAnimation();
	UpdateCollision();

	//行列の更新 
	UpdateMatrix();
}

void Player::Draw()
{
	model_.Draw();

#ifdef DEBUG_
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

void Player::Move(Input& input)
{
	vel_ = { 0.0f, 0.0f, 0.0f };
	// キーボード入力から方向作る 
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
	if (input.IsPressed("up")) inputDir.z_ += 1.0f;
	if (input.IsPressed("down")) inputDir.z_ -= 1.0f;
	if (input.IsPressed("left")) inputDir.x_ -= 1.0f;
	if (input.IsPressed("right")) inputDir.x_ += 1.0f;
	//入力があるときだけ 
	if (fabs(inputDir.x_) > 0.01f || fabs(inputDir.z_) > 0.01f)
	{
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

		while (diff > 3.141592f) diff -= 6.28318f;
		while (diff < -3.141592f) diff += 6.28318f;

		moveAngle_ += diff * 0.2f;
	}

	//アナログスティックの更新 
	//UpdateAnalogStick(input);
	
	//位置の反映
	pos_ += vel_;

	//移動の制限 
	if (pos_.x_ > kWalkLimit) pos_.x_ = kWalkLimit;
	if (pos_.x_ < -kWalkLimit) pos_.x_ = -kWalkLimit;
	if (pos_.z_ > kWalkLimit) pos_.z_ = kWalkLimit;
	if (pos_.z_ < -kWalkLimit) pos_.z_ = -kWalkLimit;
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
	invincibleTimer_ = kDodgeTime;
}

void Player::UpdateTimers()
{
	const float deltatime = 1.0f / 60.0f;

	if (attackTimer_ > 0.0f) attackTimer_ -= deltatime;
	if (dodgeTimer_ > 0.0f) dodgeTimer_ -= deltatime;
	if (invincibleTimer_ > 0.0f) invincibleTimer_ -= deltatime;
}

void Player::HandleInput(Input& input)
{
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

void Player::UpdateAction(Input& input)
{
	switch (state_)
	{
	case PlayerState::Idle:
	case PlayerState::Run:
		Move(input);
		break;

	case PlayerState::Attack:
		UpdateAttack();
		break;

	case PlayerState::Dodge:
		UpdateDodge();
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
}

void Player::UpdateAttack()
{
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

void Player::UpdateDodge()
{
	Vector3 dir = { -sinf(moveAngle_), 0.0f, cosf(moveAngle_) };

	pos_ += dir * kDodgeSpeed;

	//制限
	if (pos_.x_ > kWalkLimit) pos_.x_ = kWalkLimit;
	if (pos_.x_ < -kWalkLimit) pos_.x_ = -kWalkLimit;
	if (pos_.z_ > kWalkLimit) pos_.z_ = kWalkLimit;
	if (pos_.z_ < -kWalkLimit) pos_.z_ = -kWalkLimit;

	collider_.SetEnable(false);
	collider_.SetPos(pos_ + kColOffset);

	if (dodgeTimer_ <= 0.0f)
	{
		collider_.SetEnable(true);
		collider_.SetPos(pos_ + kColOffset);
	}
}

void Player::UpdateAnimation()
{
	AnimationState animState = AnimationState::Idle;

	switch (state_)
	{
	case PlayerState::Idle:   animState = AnimationState::Idle; break;
	case PlayerState::Run:    animState = AnimationState::Run; break;
	case PlayerState::Attack: animState = AnimationState::Attack; break;
	case PlayerState::Dodge:  animState = AnimationState::Dodge; break;
	}

	animation_.ChangeState(animState);
	animation_.Update(1.0f / 60.0f);
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
	if (IsInvincible()) return;

	// 相手がEnemyかチェック
	if (dynamic_cast<Enemy*>(other))
	{
		isHit_ = true;
	}
}

void Player::OnHit(GameObject* attacker)
{
	if (IsInvincible()) return;

	isHit_ = true;
}
