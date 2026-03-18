#include "Camera.h"
#include "../Actor/Player.h"
#include"../Physics/Matrix4x4.h"
#include<Dxlib.h>

namespace
{
	//const Vector3 kTargetToCamera = VGet(0.0f,280.0f,660.0f);
}

Camera::Camera():
	GameObject(pos_, vel_),
	cameraTarget_(0.0f,0.0f,0.0f)
{
	pPlayer_ = std::make_shared<Player>();
}

Camera::~Camera()
{

}

void Camera::Init()
{
	//カメラの注視点
	cameraTarget_ = pPlayer_->GetCameraTarget();

	//カメラはプレイヤーと同じ方向を見る
	float playerAngle = pPlayer_->GetAngle();
	Matrix4x4 cameraMat = Matrix4x4::RotateY(playerAngle);

	Vector3 targetToCamera=Matrix4x4::Transform()
}

void Camera::Update()
{
}

void Camera::UpdateCamera()
{

}
