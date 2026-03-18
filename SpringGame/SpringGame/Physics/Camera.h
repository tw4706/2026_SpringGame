#pragma once
#include"../GameObject.h"
class Camera :public GameObject
{
public:
	Camera();
	virtual~Camera();

	void Init()override;
	void Update()override;
	void Draw()override;
private:
};

