#pragma once
#include "GameObject.h"
#include"../GameObject/Model.h"
#include "../Physics/SphereCollider.h"
class GoalObject :public GameObject
{
public:
	GoalObject();
	~GoalObject();

	void Init()override;
	void Update()override;
	void Draw()override;

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other">当たり判定を行うゲームオブジェクト</param>
	void OnCollision(GameObject* other);

	/// <summary>
	/// ヒットしたかどうか
	/// </summary>
	/// <returns></returns>
	bool IsHit()const;

private:
	float angle_;
	float hitTimer_;
	bool isHit_;

	SphereCollider pCollider_;
	Model model_;
};

