#include "Player.h"
#include"../Input.h"
#include"../Physics/Matrix4x4.h"
#include"../Physics/Vector3.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//プレイヤーの移動速度
	constexpr float kSpeed = 8.0f;

	constexpr float kCameraSpeed = 0.05f;
}

Player::Player() :
	GameObject(pos_, vel_),
	modelHandle_(-1),
	cameraAngle_(0.0f),
	moveAngle_(0.0f)
{
	pos_ = { -0.0f,0.0f,0.0f };
}

Player::~Player()
{
	MV1DeleteModel(modelHandle_);
}

void Player::Init()
{
	modelHandle_ = MV1LoadModel("data/Player.mv1");
	assert(modelHandle_ >= 0);
}

void Player::Update(Input& input)
{
	//移動
	Move(input);
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);
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
		moveAngle_ = atan2f(-playerDir.x_, playerDir.z_);
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
	Matrix4x4 transMat = Matrix4x4::Translate(pos_);

	//拡縮
	Matrix4x4 scaleMat = Matrix4x4::Scale({ 100.0f,100.0f,100.0f });

	//行列の合成
	Matrix4x4 mat = scaleMat * rotMat * transMat;
	MV1SetMatrix(modelHandle_, mat.ToDxlibMatrix());
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}
