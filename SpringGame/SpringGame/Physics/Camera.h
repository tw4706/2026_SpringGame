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

	void AddRotation(float yaw, float pitch);

	//シェイク開始
	void Shake(float time, float power);

	Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
	float Lerp(float a, float b, float t);

	//ズーム開始
	void StartZoom(float scale);
	//シェイク更新
	Vector3 UpdateShake();

	Vector3 GetPos()const { return pos_; }
	float GetYaw() const { return yaw_; }

	void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }
private:
	float shakeTime_;
	float shakePower_;
	float fovTarget_;
	float fov_;
	float yaw_;
	float pitch_;

	//カメラの注視点
	Vector3 cameraTarget_;
	std::shared_ptr<Player>pPlayer_;
};

