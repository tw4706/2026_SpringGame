#include "Player.h"
#include"../Input.h"
#include"../Physics/Vector3.h"
#include<Dxlib.h>

namespace
{
	//プレイヤーの移動速度
	constexpr float kSpeed = 4.0f;
}

Player::Player():
	GameObject(pos_,vel_),
	modelHandle_(-1)
{
	pos_ = { -400.0f,0.0f,0.0f };
}

Player::~Player()
{
	MV1DeleteModel(modelHandle_);
}

void Player::Init()
{
	modelHandle_ = MV1LoadModel("data/Player.mv1");
	MV1SetPosition(modelHandle_,pos_.ToDxlibVector(pos_));
}

void Player::Update(Input&input)
{
	//移動
	Move(input);

	pos_ += vel_;
	//座標の反映
	MV1SetPosition(modelHandle_, pos_.ToDxlibVector(pos_));
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);
}

//移動
void Player::Move(Input&input)
{
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
}

//攻撃
void Player::Attack(Input& input)
{
}
