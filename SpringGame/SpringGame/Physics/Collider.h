#pragma once
#include"Vector3.h"

//“–‚½‚è”»’è‚Ìí—Ş
enum class CollisionType
{
	Sphere,
	Capsule,
};

class Collider
{
public:
	virtual ~Collider() = default;

	//“–‚½‚è”»’è‚Ìí—Ş‚ğæ“¾‚·‚é
	virtual CollisionType GetCollisionType()const abstract;

protected:
	Vector3 centerPos_;	//“–‚½‚è”»’è‚Ì’†SÀ•W
	void* owner;		//“–‚½‚è”»’è‚Ì‚¿å
};

