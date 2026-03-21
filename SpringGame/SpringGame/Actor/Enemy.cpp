#include "Enemy.h"
#include "../Physics/Vector3.h"
#include "../Actor/Player.h"
#include<Dxlib.h>
#include<cassert>

namespace
{
	//当たり判定のサイズ
	constexpr float kColSize = 50.0f;

	//初期位置
	const Vector3 kFirstPos = { 400.0f,0.0f,0.0f };

	//モデルのサイズ
	const VECTOR kModelScale = { 100.0f,100.0f, 100.0f };
}

Enemy::Enemy() :
	GameObject(pos_, vel_),
	modelHandle_(-1),
	collider_(kColSize),
	isHit_(false)
{
	pos_ = kFirstPos;
	collider_.SetOwner(this);
}

Enemy::~Enemy()
{
	MV1DeleteModel(modelHandle_);
}

void Enemy::Init()
{
	modelHandle_ = MV1LoadModel("data/Enemy.mv1");
	assert(modelHandle_ >= 0);
	MV1SetPosition(modelHandle_, pos_.ToDxlibVector());
	MV1SetScale(modelHandle_, kModelScale);
}

void Enemy::Update()
{
	isHit_ = false;

	collider_.SetPos(pos_ + Vector3(0.0f, 100.0f, 0.0f));
}

void Enemy::Draw()
{
	MV1DrawModel(modelHandle_);

	// 当たり判定の描画
	unsigned int color = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

	DrawSphere3D(
		collider_.GetPos().ToDxlibVector(), // 中心
		collider_.GetRadian(),				// 半径
		16, color, color, FALSE);
}

void Enemy::OnCollision(GameObject* other)
{
	// 相手がPlayerかチェック
	if (dynamic_cast<Player*>(other))
	{
		isHit_ = true;
	}
}
