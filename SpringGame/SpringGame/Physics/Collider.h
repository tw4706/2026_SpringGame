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

	//当たり判定の種類を取得する
	virtual CollisionType GetCollisionType()const abstract;

	//衝突判定
	virtual void OnCollision(Collider* other) {}

	//位置の取得
	Vector3 GetPos()const { return pos_; }
	void SetPos(const Vector3& pos) { pos_ = pos; }

	//当たり判定の種類
	void SetColliderType(ColliderType type) { type_ = type; }
	ColliderType GetColliderType() const { return type_; }

	//持ち主
	void SetOwner(GameObject* owner) { owner_ = owner; }
	GameObject* GetOwner() const { return owner_; }

	//当たり判定の有効・無効
	void SetEnable(bool flag) { isEnable_ = flag; }
	bool IsEnable() const { return isEnable_; }

protected:
	Vector3 pos_;		//当たり判定の中心座標
	GameObject* owner_;//当たり判定の持ち主
	ColliderType type_;
	bool isEnable_ = true;
};

