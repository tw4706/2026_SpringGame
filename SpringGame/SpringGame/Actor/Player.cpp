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

	// プレイヤー基準位置から注視点までのベクトル
	//const Vector3 kPlayerToTarget = { 0.0f, 290.0f, 0.0f };
}

Player::Player():
	GameObject(pos_,vel_),
	modelHandle_(-1),
	angle_(0.0f)
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

void Player::Update(Input&input)
{
	//移動
	Move(input);

	vel_ = { 0.0f,0.0f,0.0f };

	//入力に応じて速度を入れる
	if (input.IsPressed("up"))
	{
		vel_.z_ += kSpeed;
	}
	if (input.IsPressed("down"))
	{
		vel_.z_ -= kSpeed;
	}
	if (input.IsPressed("left"))
	{
		vel_.x_ -= kSpeed;
	}
	if (input.IsPressed("right"))
	{
		vel_.x_ += kSpeed;
	}
	if (input.IsPressed("cameraLeft"))
	{
		angle_ -= kCameraSpeed;
	}
	if (input.IsPressed("cameraRight"))
	{
		angle_ += kCameraSpeed;
	}

	//回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(angle_);
	vel_ = rotMat.TransformForVector(vel_);

	pos_ += vel_;

	//移動
	Matrix4x4 transMat = Matrix4x4::Translate(pos_);

	//拡縮
	Matrix4x4 scaleMat = Matrix4x4::Scale({ 100.0f,100.0f,100.0f });

	//行列の合成
	Matrix4x4 mat = scaleMat*rotMat * transMat;
	MV1SetMatrix(modelHandle_, mat.ToDxlibMatrix());
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);
}

//移動
void Player::Move(Input&input)
{
}

//攻撃
void Player::Attack(Input& input)
{
}

Vector3 Player::GetCameraTarget() const
{
	return pos_;
}
