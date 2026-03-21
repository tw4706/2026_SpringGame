#include "SphereCollider.h"

SphereCollider::SphereCollider(float r):
	r_(r)
{
}

CollisionType SphereCollider::GetCollisionType() const
{
	return CollisionType::Sphere;
}
