#include "CollisionManager.h"
#include"SphereCollider.h"

void CollisionManager::AddCollider(Collider* col)
{
	//“–‚½‚è”»’è‚Ì“o˜^
	colliders_.push_back(col);
}

void CollisionManager::Clear()
{
	//“–‚½‚è”»’è‚Ì‰Šú‰»
	colliders_.clear();
}

bool CollisionManager::CheckSphereSphere(SphereCollider* a, SphereCollider* b)
{
	Vector3 diff = a->GetPos() - b->GetPos();

	float distSq = diff.LengthSq();
	float r = a->GetRadian() + b->GetRadian();

	return distSq <= r * r;
}

void CollisionManager::CheckAllCollision()
{

	for (size_t i = 0; i < colliders_.size(); i++)
	{
		for (size_t j = i + 1; j < colliders_.size(); j++)
		{
			Collider* a = colliders_[i];
			Collider* b = colliders_[j];

			ColliderType typeA = a->GetColliderType();
			ColliderType typeB = b->GetColliderType();

			if (!a->IsEnable() || !b->IsEnable()) continue;

			if (typeA == ColliderType::Charactor && typeB == ColliderType::Charactor)
			{
				if (a->GetOwner() == b->GetOwner()) continue;
			}

			//Ž©g‚ÌUŒ‚”»’è‚à–³Ž‹
			if (typeA == ColliderType::Attack && typeB == ColliderType::Charactor)
			{
				if (a->GetOwner() == b->GetOwner()) continue;
			}
			if (typeB == ColliderType::Attack && typeA == ColliderType::Charactor)
			{
				if (a->GetOwner() == b->GetOwner()) continue;
			}

			//Œ^‚Å•ªŠò
			if (a->GetCollisionType() == CollisionType::Sphere &&
				b->GetCollisionType() == CollisionType::Sphere)
			{
				if (CheckSphereSphere(
					static_cast<SphereCollider*>(a),
					static_cast<SphereCollider*>(b)))
				{
					//Õ“Ë’Ê’m
					a->OnCollision(b);
					b->OnCollision(a);
				}
			}
		}
	}
}
