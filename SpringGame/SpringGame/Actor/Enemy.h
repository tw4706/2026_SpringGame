#pragma once
#include "../GameObject.h"
#include "../Physics/SphereCollider.h"

class Enemy :public GameObject
{
public:
	Enemy();
	virtual~Enemy();

	void Init()override;
	void Update()override;
	void Draw()override;

	SphereCollider* GetCollider() { return &collider_; }

	void OnCollision(GameObject* other);

private:
	int modelHandle_;
	SphereCollider collider_;
	bool isHit_;
};

