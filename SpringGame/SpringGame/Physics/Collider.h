#pragma once
#include"Vector3.h"

//当たり判定の種類
enum class CollisionType
{
	Sphere,
	Capsule,
};

enum class ColliderType
{
	Charactor,
	Attack
};

class GameObject;
class Collider
{
public:
	virtual ~Collider() = default;

	/// <summary>
	/// 当たり判定の種類を取得する
	/// </summary>
	/// <returns>当たり判定の種類を返す</returns>
	virtual CollisionType GetCollisionType()const abstract;

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollision(Collider* other) {}

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>当たり判定の座標を返す</returns>
	Vector3 GetPos()const { return pos_; }

	/// <summary>
	/// 座標のセット
	/// </summary>
	/// <param name="pos">座標のセット</param>
	void SetPos(const Vector3& pos) { pos_ = pos; }

	/// <summary>
	/// 当たり判定の種類のセット
	/// </summary>
	/// <param name="type">当たり判定の種類</param>
	void SetColliderType(ColliderType type) { type_ = type; }

	/// <summary>
	/// 当たり判定の種類の取得
	/// </summary>
	/// <returns>当たり判定の種類を返す</returns>
	ColliderType GetColliderType() const { return type_; }

	/// <summary>
	/// コライダーの持ち主のセット
	/// </summary>
	/// <param name="owner">コライダーの持ち主</param>
	void SetOwner(GameObject* owner) { owner_ = owner; }

	/// <summary>
	/// コライダーの取得
	/// </summary>
	/// <returns>持ち主を返す</returns>
	GameObject* GetOwner() const { return owner_; }

	/// <summary>
	/// 当たり判定の有効・無効
	/// </summary>
	/// <param name="flag">当たり判定が有効かどうか</param>
	void SetEnable(bool flag) { isEnable_ = flag; }

	/// <summary>
	/// 当たり判定が有効かどうか
	/// </summary>
	/// <returns>当たり判定が有効ならtrue,有効でないならfalse</returns>
	bool IsEnable() const { return isEnable_; }

protected:
	Vector3 pos_;		//当たり判定の中心座標
	GameObject* owner_;//当たり判定の持ち主
	ColliderType type_;
	bool isEnable_ = true;
};

