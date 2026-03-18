#pragma once
#include "GameObject.h"
class Enemy :public GameObject
{
public:
	Enemy();
	virtual~Enemy();

	void Init()override;
	void Update()override;
	void Draw()override;

private:
	int modelHandle_;
};

