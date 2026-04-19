#include "GoalObject.h"

namespace
{
	const Vector3 kFirstPos = { 0.0f,0.0f,10000.0f };

	const Vector3 kModelScale = { 5.0f,5.0f,5.0f };

	//当たり判定の位置調整
	const Vector3 kColOffset = { 0.0f,80.0f,0.0f };
}

GoalObject::GoalObject() :
	GameObject(pos_,vel_),
	angle_(0.0f)
{
	pos_ = kFirstPos;
	//pCollider_.SetOwner(this);

	//コライダーを登録
	//GameObject::pCollider_ = &pCollider_;
}

GoalObject::~GoalObject()
{
	model_.Release();
}

void GoalObject::Init()
{
	model_.Load("data/goalObject.mv1");
	model_.SetPosition(pos_);
	model_.SetScale(kModelScale);

	//当たり判定の初期化
	//pCollider_.SetEnable(false);
	//pCollider_.SetColliderType(ColliderType::Charactor);
	//pCollider_.SetPos(pos_ + kColOffset);
}

void GoalObject::Update()
{
	angle_ += 0.02f;
}

void GoalObject::Draw()
{
	model_.SetPosition(pos_);
	model_.SetScale(kModelScale);

	//モデルの回転
	model_.SetRotationY(angle_);

	model_.Draw();
}
