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
    //“–‚½‚è”»’è‚ª–³Œø‚È‚ç–³Ž‹
    if (!IsEnable() || !other->IsEnable()) return;

    //Ž©•ªŽ©g‚Í–³Ž‹
    if (GetOwner() == other->GetOwner()) return;

    //Attack ¨ Body
    if (GetColliderType() == ColliderType::Attack &&
        other->GetColliderType() == ColliderType::Charactor)
    {
        other->GetOwner()->OnHit(GetOwner()); //UŒ‚ŽÒ‚à“n‚·
    }

    //Body ¨ Attack
    if (GetColliderType() == ColliderType::Charactor &&
        other->GetColliderType() == ColliderType::Attack)
    {
        GetOwner()->OnHit(other->GetOwner());
    }

    //–{‘Ì“¯Žm
    if (GetColliderType() == ColliderType::Charactor &&
        other->GetColliderType() == ColliderType::Charactor)
    {
        GetOwner()->OnCollision(other->GetOwner());
    }
}
