#pragma once
#include "Collider.h"
class SphereCollider :public Collider
{
public:
	SphereCollider(float r);

	CollisionType GetCollisionType()const override;

	//”¼Œa‚ÌŽæ“¾
	float GetRadian()const { return r_; }

	void OnCollision(Collider* other);

private:
	float r_;//”¼Œa
};

