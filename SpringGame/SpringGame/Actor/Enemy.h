#pragma once
#include "../GameObject.h"
#include "../Physics/SphereCollider.h"

class Player;
class Enemy :public GameObject
{
public:
	Enemy();
	virtual~Enemy();

	void Init()override;
	void Update()override;
	void Draw()override;

	void OnHit(GameObject* attacker)override;

	SphereCollider* GetCollider() { return &collider_; }

	void OnCollision(GameObject* other);

	void SetPlayer(Player* player) { pPlayer_ = player; }

private:
	int modelHandle_;
	SphereCollider collider_;
	bool isHit_;
	float hitTimer_;

	Player* pPlayer_=nullptr;//プレイヤーのポインタ(参照用)
};

