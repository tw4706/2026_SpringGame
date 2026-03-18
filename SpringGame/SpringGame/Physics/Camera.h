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
	void Draw()override {};

	void UpdateCamera();

	void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }
private:
	//ƒJƒƒ‰‚Ì’‹“_
	Vector3 cameraTarget_;
	std::shared_ptr<Player>pPlayer_;
};

