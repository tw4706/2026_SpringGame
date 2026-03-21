#pragma once
#include <vector>
#include "Collider.h"

class SphereCollider;
class CollisionManager
{
public:
	void AddCollider(Collider* col);

	//初期化
	void Clear();

	//球と球の当たり判定
	bool CheckSphereSphere(SphereCollider* a, SphereCollider* b);

	//全当たり判定のチェック
	void CheckAllCollision();

private:
	//コライダーの配列(格納する場所)
	std::vector<Collider*>colliders_;
};

