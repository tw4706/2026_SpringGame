#pragma once
#include "Collider.h"
class SphereCollider :public Collider
{
public:
	SphereCollider(float r);

	/// <summary>
	/// 当たり判定の種類の取得
	/// </summary>
	/// <returns>当たり判定の種類</returns>
	CollisionType GetCollisionType()const override;

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <returns>半径</returns>
	float GetRadian()const { return r_; }

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other">コライダーの名前</param>
	void OnCollision(Collider* other);

private:
	float r_;//半径
};

