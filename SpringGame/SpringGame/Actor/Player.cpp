#include "Player.h"
#include"../Input.h"
#include"../Physics/Matrix4x4.h"
#include"../Physics/Vector3.h"
#include "../Actor/Enemy.h"
#include<Dxlib.h>
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

	const Vector3 kColOffset = { 0.0f,100.0f,0.0f };

	//モデルのサイズ
	constexpr float kModelScal = 100.0f;
}

Player::Player() :
	GameObject(pos_, vel_),
	cameraAngle_(0.0f),
	moveAngle_(0.0f),
	attackTimer_(0.0f),
	isAttacking_(false),
	collider_(0.0f),
	attackCollider_(0.0f),
	isHit_(false)
{
	pos_ = kFirstPos;
	collider_ = SphereCollider(kColSize);
	attackCollider_ = SphereCollider(kAttackColSize);
	collider_.SetOwner(this);
	attackCollider_.SetOwner(this);
}

Player::~Player()
{
	model_.Release();
}

void Player::Init()
{
	model_.Load("data/Player.mv1");

	animation_.Init(model_.GetHandle());
	animation_.ChangeState(AnimationState::Idle);

	collider_.SetEnable(true);
	attackCollider_.SetEnable(false);

	collider_.SetColliderType(ColliderType::Charactor);
	attackCollider_.SetColliderType(ColliderType::Attack);
}

void Player::Update(Input& input)
{
	isHit_ = false;

	collider_.SetPos(pos_ + kColOffset);

	//移動
	Move(input);

	//攻撃
	Attack(input);

	state_ = GetState();
	animation_.ChangeState(state_);

	//アニメーションの更新
	animation_.Update(1.0f / 60.0f);
}

void Player::Draw()
{
	model_.Draw();

	//当たり判定の描画
	unsigned int color = isHit_ ? 0xff0000 : 0x00ff00;

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(),		//中心
		collider_.GetRadian(),					//半径
		16, color, color, FALSE);

	//攻撃判定の描画
	if (isAttacking_)
	{
		DrawSphere3D(
			attackCollider_.GetPos().ToDxlibVector(),
			attackCollider_.GetRadian(),
			16,
			0xffff00,
			0xffff00,
			FALSE);
	}
}

//移動
void Player::Move(Input& input)
{
	vel_ = { 0.0f, 0.0f, 0.0f };

	// キーボード入力から方向作る
	Vector3 inputDir = { 0.0f, 0.0f, 0.0f };

	if (input.IsPressed("up"))    inputDir.z_ += 1.0f;
	if (input.IsPressed("down"))  inputDir.z_ -= 1.0f;
	if (input.IsPressed("left"))  inputDir.x_ -= 1.0f;
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

	//行列の更新
	UpdateMatrix();
}

//攻撃
void Player::Attack(Input& input)
{
	//ボタン押した瞬間
	if (input.IsTriggered("attack") && !isAttacking_)
	{
		isAttacking_ = true;
		attackTimer_ = 0.5f; //攻撃時間

		attackCollider_.SetEnable(true);
	}

	//攻撃中
	if (isAttacking_)
	{
		attackTimer_ -= 1.0f / 60.0f;

		//プレイヤーの前方向
		Vector3 forward = {-sinf(moveAngle_),0.0f,cosf(moveAngle_)};

		//攻撃位置
		Vector3 attackPos = pos_ + forward * 100.0f + kColOffset;

		attackCollider_.SetPos(attackPos);

		//時間終了で攻撃終了
		if (attackTimer_ <= 0.0f)
		{
			isAttacking_ = false;
			attackCollider_.SetEnable(false);
		}
	}
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
	Matrix4x4 scaleMat = Matrix4x4::Scale(kModelScal, kModelScal, kModelScal);

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
	// 相手がEnemyかチェック
	if (dynamic_cast<Enemy*>(other))
	{
		isHit_ = true;
	}
}

AnimationState Player::GetState() const
{
	//攻撃
	if (isAttacking_)
	{
		return AnimationState::Attack;
	}

	//移動
	bool isMoving = (fabs(vel_.x_) > 0.1f || fabs(vel_.z_) > 0.1f);

	if (isMoving)
	{
		return AnimationState::Run;
	}

	return AnimationState::Idle;
}

void Player::OnHit(GameObject* attacker)
{
	isHit_ = true;
}
