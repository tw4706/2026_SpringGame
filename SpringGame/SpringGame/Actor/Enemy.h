#pragma once
#include"../Model.h"
#include"../Animation.h"
#include "../GameObject.h"
#include "../Physics/SphereCollider.h"

class Player;
class Camera;
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

	void SetPos(const Vector3& pos) { pos_ = pos; }

	void SetCamera(Camera* camera) { pCamera_ = camera; }

	bool IsDestroy()const { return isDestroy_; }
private:
	AnimationState GetState()const;

private:
	SphereCollider collider_;
	bool isHit_;
	float hitTimer_;
	bool isDead_;
	bool isDestroy_;
	bool isSpawning_;

	Player* pPlayer_=nullptr;//プレイヤーのポインタ(参照用)
	Camera* pCamera_ = nullptr;
	Model model_;
	Animation animation_;
	AnimationState state_;
};

