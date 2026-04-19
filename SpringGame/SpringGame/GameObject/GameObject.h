#pragma once
#include"../Physics/Vector3.h"

class Collider;
class GameObject
{
public :
	GameObject(Vector3 pos,Vector3 vel);
	virtual~GameObject();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;

	///衝突判定
	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other">衝突した相手のオブジェクト</param>
	virtual void OnCollision(GameObject* other) {}

	/// <summary>
	/// ヒット時の処理
	/// </summary>
	/// <param name="attacker">攻撃してきたオブジェクト</param>
	virtual void OnHit(GameObject* attacker) {};

	//当たり判定の取得

	/// <summary>
	/// 当たり判定の取得
	/// </summary>
	/// <returns>当たり判定のポインタ</returns>
	Collider* GetCollider()const { return pCollider_; }

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標の参照</returns>
	const Vector3& GetPos() const { return pos_; }

protected:
	Vector3 pos_;
	Vector3 vel_;
	float dir_;
	Collider* pCollider_ = nullptr;
};

