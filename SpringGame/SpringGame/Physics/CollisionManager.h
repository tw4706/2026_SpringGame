#pragma once
#include <vector>
#include "Collider.h"

class SphereCollider;
class CollisionManager
{
public:
	//当たり判定の登録
	void AddCollider(Collider* col);

	//初期化
	void Clear();

	//球と球の当たり判定
	bool CheckSphereSphere(SphereCollider* a, SphereCollider* b);

	//全当たり判定のチェック
	void CheckAllCollision();

private:
	//当たり判定のリスト
	std::vector<Collider*>colliders_;
};

