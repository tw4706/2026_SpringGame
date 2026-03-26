#include "Camera.h"
#include "../Actor/Player.h"
#include"../Physics/Matrix4x4.h"
#include<Dxlib.h>

namespace
{
	const Vector3 kTargetToCamera = { 0.0f,280.0f,-660.0f };
}

Camera::Camera():
	GameObject(pos_, vel_),
	angle_(0.0f),
	shakeTime_(0.0f),
	shakePower_(0.0f),
	cameraTarget_(0.0f,0.0f,0.0f)
{
}

Camera::~Camera()
{

}

void Camera::Init()
{
	cameraTarget_ = pPlayer_->GetCameraTarget();

	float playerAngle = pPlayer_->GetCameraAngle();
	Matrix4x4 rotMat = Matrix4x4::RotateY(playerAngle);
	Vector3 offset = rotMat.TransformForVector(kTargetToCamera);

	pos_ = cameraTarget_ + offset;

	SetCameraPositionAndTarget_UpVecY(pos_.ToDxlibVector(),cameraTarget_.ToDxlibVector());
}

void Camera::Update()
{
	//プレイヤーがいないときは更新しない
	if (!pPlayer_)return;

	UpdateCamera();
}

void Camera::UpdateCamera()
{
	//カメラの注視点
	cameraTarget_ = pPlayer_->GetCameraTarget();

	//カメラはプレイヤーと同じ方向を見る
	float playerAngle = pPlayer_->GetCameraAngle();

	//回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(playerAngle);
	Vector3 offset = rotMat.TransformForVector(kTargetToCamera);

	//カメラの位置
	Vector3 cameraPos = cameraTarget_ + offset;

	//カメラの補間
	pos_ += (cameraPos - pos_) * 0.1f;

	//カメラのシェイク適用
	pos_ += UpdateShake();

	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(pos_.ToDxlibVector(), cameraTarget_.ToDxlibVector());
}

void Camera::Shake(float time, float power)
{
	shakeTime_ = time;
	shakePower_ = power;
}

Vector3 Camera::UpdateShake()
{
	if (shakeTime_ <= 0.0f)
	{
		return Vector3(0, 0, 0);
	}

	shakeTime_ -= 1.0f / 60.0f;

	float rx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
	float ry = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
	float rz = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

	Vector3 shake = {rx * shakePower_,ry * shakePower_,rz * shakePower_};

	return shake;
}
