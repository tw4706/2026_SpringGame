#include "GoalObject.h"

namespace
{
	const Vector3 kFirstPos = { 0.0f,0.0f,10000.0f };
	const Vector3 kModelScale = { 10.0f,10.0f,10.0f };
}

GoalObject::GoalObject() :
	GameObject(pos_,vel_),
	angle_(0.0f)
{
	pos_ = kFirstPos;
	pCollider_->SetOwner(this);

	//コライダーを登録
	GameObject::pCollider_ = pCollider_;
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
