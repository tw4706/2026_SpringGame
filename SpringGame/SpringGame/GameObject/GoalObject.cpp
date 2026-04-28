#include "GoalObject.h"
#include"../Scene/ResultScene.h"
#include"../Scene/SceneController.h"
#include<memory>

namespace
{
	//モデルの初期位置
	const Vector3 kFirstPos = { 0.0f,0.0f,10000.0f };

	//モデルの座標オフセット
	const Vector3 kPosOffset = { 0.0f,150.0f,0.0f };

	//モデルの拡大率
	const Vector3 kModelScale = { 2.0f,1.5f,2.0f };

	//当たり判定のサイズ
	constexpr float kColSize = 160.0f;

	//当たり判定の位置調整
	const Vector3 kColOffset = { 0.0f,80.0f,0.0f };
}

GoalObject::GoalObject() :
	GameObject(pos_, vel_),
	angle_(0.0f),
	hitTimer_(0.0f),
	isHit_(false),
	pCollider_(kColSize)
{
	pos_ = kFirstPos;
	pCollider_.SetOwner(this);

	//コライダーを登録
	GameObject::pCollider_ = &pCollider_;
}

GoalObject::~GoalObject()
{
	model_.Release();
}

void GoalObject::Init()
{
	model_.Load("data/Model/goalObject.mv1");
	model_.SetPosition(pos_ + kPosOffset);
	model_.SetScale(kModelScale);

	//当たり判定の初期化
	pCollider_.SetEnable(true);
	pCollider_.SetColliderType(ColliderType::Charactor);
	pCollider_.SetPos(pos_ + kColOffset);
}

void GoalObject::Update()
{
	angle_ += 0.02f;

	//モデルのセットポジション
	model_.SetPosition(pos_+kPosOffset);

	//当たり判定の位置更新
	pCollider_.SetPos(pos_ + kColOffset);
}

void GoalObject::Draw()
{
	//モデルの回転
	model_.SetRotationY(angle_);

	model_.Draw();
#ifdef _DEBUG
	unsigned int color = isHit_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

	//敵の当たり判定描画
	DrawSphere3D(
		pCollider_.GetPos().ToDxlibVector(),	//中心
		pCollider_.GetRadian(),					//半径
		16, color, color, FALSE);
#endif
}

void GoalObject::OnCollision(GameObject*other)
{
	if (other->GetCollider()->GetColliderType() == ColliderType::Charactor)
	{
		isHit_ = true;
	}
}

bool GoalObject::IsHit() const
{
	return isHit_;
}
