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

	//シェイク開始
	void Shake(float time, float power);
	//シェイク更新
	Vector3 UpdateShake();

	void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }
private:
	float angle_;
	float shakeTime_;
	float shakePower_;

	//カメラの注視点
	Vector3 cameraTarget_;
	std::shared_ptr<Player>pPlayer_;
};

