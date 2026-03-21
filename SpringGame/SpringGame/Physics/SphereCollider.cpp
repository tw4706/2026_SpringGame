#include "SphereCollider.h"
#include "../GameObject.h"

SphereCollider::SphereCollider(float r):
	r_(r)
{
}

CollisionType SphereCollider::GetCollisionType() const
{
	return CollisionType::Sphere;
}

void SphereCollider::OnCollision(Collider* other)
{
	if (owner_ && other->GetOwner())
	{
		owner_->OnCollision(other->GetOwner());
	}
}
