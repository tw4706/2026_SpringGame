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
	cameraTarget_(0.0f,0.0f,0.0f)
{
}

Camera::~Camera()
{

}

void Camera::Init()
{
	cameraTarget_ = pPlayer_->GetCameraTarget();

	float playerAngle = pPlayer_->GetAngle();
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
	float playerAngle = pPlayer_->GetAngle();

	//回転
	Matrix4x4 rotMat = Matrix4x4::RotateY(-playerAngle);
	Vector3 offset = rotMat.Transform(kTargetToCamera);

	//カメラの位置
	Vector3 cameraPos = cameraTarget_ + offset;

	//カメラの補間
	pos_ += (cameraPos - pos_) * 0.1f;

	//カメラの設定
	SetCameraPositionAndTarget_UpVecY(pos_.ToDxlibVector(), cameraTarget_.ToDxlibVector());
}
