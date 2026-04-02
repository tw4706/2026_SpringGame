#pragma once
#include"../Physics/Vector3.h"
#include<Dxlib.h>
#include<array>

class Bg
{
public:
	Bg();
	virtual~Bg();

	void Init();
	void Draw(const Vector3&cameraPos);
private:
	std::array<int,6>skyTex_{};
};

