#pragma once
#include "Collider.h"
class SphereCollider :public Collider
{
public:
	SphereCollider(float r);

	CollisionType GetCollisionType()const override;

private:
	float r_;//”¼Œa
};

