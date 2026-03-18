#pragma once
#include "../GameObject.h"

class Input;
class Player :public GameObject
{
public:
	Player();
	virtual~Player();

	void Init()override;
	void Update()override{};
	void Update(Input&input);
	void Draw()override;

	void Move(Input& input);
	void Attack(Input& input);

	//プレイヤーの位置を取得
	Vector3 GetPos()const{ return pos_; }
	//カメラ用の角度を取得
	float GetCameraAngle()const { return cameraAngle_; }

	//カメラの注視点の取得
	Vector3 GetCameraTarget()const;

private:
	int modelHandle_;
	float cameraAngle_; //カメラ用の角度
	float moveAngle_;	//プレイヤーの移動用の角度
};

