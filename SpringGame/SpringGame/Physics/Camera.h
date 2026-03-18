#pragma once
#include"../GameObject.h"
#include<memory>

class Player;
class Camera :public GameObject
{
public:
	Camera();
	virtual~Camera();

	void Init()override;
	void Update()override;

	void UpdateCamera();
private:
	//ƒJƒƒ‰‚Ì’‹“_
	Vector3 cameraTarget_;
	std::shared_ptr<Player>pPlayer_;
};

