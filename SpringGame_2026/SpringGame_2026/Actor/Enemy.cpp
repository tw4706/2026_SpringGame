#include "Enemy.h"
#include "Vector3.h"
#include<Dxlib.h>

Enemy::Enemy() :
	GameObject(pos_, vel_),
	modelHandle_(-1)
{
	pos_ = { 400.0f,0.0f,0.0f };
}

Enemy::~Enemy()
{
	MV1DeleteModel(modelHandle_);
}

void Enemy::Init()
{
	modelHandle_ = MV1LoadModel("data/Enemy.mv1");
	MV1SetPosition(modelHandle_, pos_.ToDxlibVector(pos_));
	MV1SetScale(modelHandle_, VGet(100.0f, 100.0f, 100.0f));
}

void Enemy::Update()
{
}

void Enemy::Draw()
{
	MV1DrawModel(modelHandle_);
}
