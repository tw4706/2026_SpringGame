#pragma once
#include "../GameObject.h"
#include "../Physics/SphereCollider.h"
#include"../Animation.h"
#include"../Model.h"

class Input;
class Player :public GameObject
{
public:
	enum class PlayerState
	{
		Idle,
		Run,
		Attack,
		Dodge
	};
	Player();
	virtual~Player();

	void Init()override;
	void Update()override{};
	void Update(Input&input);
	void Draw()override;

	void OnHit(GameObject* attacker)override;

	//移動処理
	void Move(Input& input);
	//攻撃処理

	//回避処理


	void StartAttack();
	void StartDodge();

	void UpdateTimers();
	void HandleInput(Input&input);
	void UpdateAction(Input&input);
	void UpdateState();
	void UpdateAttack();
	void UpdateDodge();
	void UpdateAnimation();
	void UpdateCollision();
	//アナログスティックの更新処理
	void UpdateAnalogStick(Input& input);
	//行列の更新処理
	void UpdateMatrix();

	//プレイヤーの位置を取得
	Vector3 GetPos()const{ return pos_; }
	//カメラ用の角度を取得
	float GetCameraAngle()const { return cameraAngle_; }
	//カメラの注視点の取得
	Vector3 GetCameraTarget()const;

	//当たり判定の取得
	SphereCollider* GetCollider() { return &collider_; }
	SphereCollider* GetAttackCollider() { return &attackCollider_; }

	void OnCollision(GameObject* other);
	bool IsInvincible()const { return invincibleTimer_ > 0.0f; }

private:
	float cameraAngle_; //カメラ用の角度
	float moveAngle_;	//プレイヤーの移動用の角度
	bool isHit_;//衝突判定用フラグ
	float attackTimer_;
	float dodgeTimer_;
	float invincibleTimer_;

	SphereCollider collider_;		//当たり判定用変数
	SphereCollider attackCollider_;	//攻撃の当たり判定用変数
	Animation animation_;
	Model model_;						//モデル
	PlayerState state_;			//プレイヤーの状態
};

