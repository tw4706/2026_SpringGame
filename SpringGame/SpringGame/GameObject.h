#pragma once
#include"Physics/Vector3.h"

class Collider;
class GameObject
{
public :
	GameObject(Vector3 pos,Vector3 vel);
	virtual~GameObject();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;

	//Õ“Ë”»’è
	virtual void OnCollision(GameObject* other) {}
	virtual void OnHit(GameObject* attacker) {};

	//“–‚½‚è”»’è‚Ìæ“¾
	Collider* GetCollider()const { return pCollider_; }
	const Vector3& GetPos() const { return pos_; }

protected:
	Vector3 pos_;
	Vector3 vel_;
	float dir_;
	Collider* pCollider_ = nullptr;
};

