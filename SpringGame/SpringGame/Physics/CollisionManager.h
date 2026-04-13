#pragma once
#include <vector>
#include "Collider.h"

class SphereCollider;
class CollisionManager
{
public:
	//当たり判定の登録

	/// <summary>
	/// 当たり判定の登録
	/// </summary>
	/// <param name="col">当たり判定の名前</param>
	void AddCollider(Collider* col);

	//初期化
	void Clear();

	/// <summary>
	/// 球と球の当たり判定
	/// </summary>
	/// <param name="a">コライダーa</param>
	/// <param name="b">コライダーb</param>
	/// <returns>当たり判定の距離がa+bより小さいならtrue,大きいならfalse</returns>
	bool CheckSphereSphere(SphereCollider* a, SphereCollider* b);

	/// <summary>
	/// 全ての当たり判定のチェック
	/// </summary>
	void CheckAllCollision();

private:
	//当たり判定のリスト
	std::vector<Collider*>colliders_;
};

