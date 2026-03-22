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

	const Vector3 kColOffset = { 0.0f,100.0f,0.0f };

	//モデルのサイズ
	constexpr float kModelScal = 100.0f;
}

Player::Player() :
	GameObject(pos_, vel_),
	modelHandle_(-1),
	cameraAngle_(0.0f),
	moveAngle_(0.0f),
	collider_(kColSize),
	isHit_(false)
{
	pos_ = kFirstPos;
	collider_.SetOwner(this);
}

Player::~Player()
{
	MV1DeleteModel(modelHandle_);
}

void Player::Init()
{
	modelHandle_ = MV1LoadModel("data/Player.mv1");
	assert(modelHandle_ >= 0);

	animation_.Init(modelHandle_);
	animation_.ChangeState(AnimationState::Idle);
}

void Player::Update(Input& input)
{
	isHit_ = false;

	collider_.SetPos(pos_ + kColOffset);

	//移動
	Move(input);

	//アニメーションの更新
	animation_.Update(1.0f / 60.0f);
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);

	// 当たり判定の描画
	unsigned int color = isHit_ ? 0xff0000 : 0x00ff00;

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(),  // 中心
		collider_.GetRadian(),				// 半径
		16, color, color, FALSE);
}

//移動
void Player::Move(Input& input)
{
	vel_ = { 0.0f,0.0f,0.0f };

	//入力に応じて速度を入れる
	if (input.IsPressed("up"))				vel_.z_ += kSpeed;
	if (input.IsPressed("down"))			vel_.z_ -= kSpeed;
	if (input.IsPressed("left"))			vel_.x_ -= kSpeed;
	if (input.IsPressed("right"))			vel_.x_ += kSpeed;

	//アナログスティックの更新
	UpdateAnalogStick(input);

	//位置の反映
	pos_ += vel_;

	//行列の更新
	UpdateMatrix();

	//移動してるかどうかでアニメーションを変更する
	if (fabs(vel_.x_) > 0.01f || fabs(vel_.z_) > 0.01f)
	{
		animation_.ChangeState(AnimationState::Run);
	}
	else
	{
		animation_.ChangeState(AnimationState::Idle);
	}
}

//攻撃
void Player::Attack(Input& input)
{
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
	MV1SetMatrix(modelHandle_, mat.ToDxLibMatrix());
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
