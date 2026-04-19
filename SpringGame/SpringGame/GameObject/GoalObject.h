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
private:
	float angle_;
	float hitTimer_;
	bool isHit_;

	SphereCollider pCollider_;
	Model model_;
};

