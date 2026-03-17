#pragma once
#include "GameObject.h"
class Player :public GameObject
{
public:
	Player();
	virtual~Player();

	void Init()override;
	void Update()override;
	void Draw()override;

private:
	int modelHandle_;
};

