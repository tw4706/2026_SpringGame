#include "Player.h"
#include "Vector3.h"
#include<Dxlib.h>

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

void Player::Update()
{
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);
}
