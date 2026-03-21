#pragma once
#include"Vector3.h"

//当たり判定の種類
enum class CollisionType
{
	Sphere,
	Capsule,
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

	//持ち主のセット
	void SetOwner(GameObject* owner)
	{
		owner_ = owner;
	}

	//持ち主の取得
	GameObject* GetOwner() const
	{
		return owner_;
	}

protected:
	Vector3 pos_;		//当たり判定の中心座標
	GameObject* owner_;//当たり判定の持ち主
};

