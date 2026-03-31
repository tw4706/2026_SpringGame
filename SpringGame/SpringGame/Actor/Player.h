#pragma once
#include "../GameObject.h"
#include "../Physics/SphereCollider.h"
#include"../Animation.h"
#include"../Model.h"

struct AfterImage
{
	int modelHandle;
	Vector3 pos;
	float angle;
	float life;
};

class Input;
class Player :public GameObject
{
public:
	//プレイヤーの状態
	enum class PlayerState
	{
		Idle,	//待機
		Run,	//移動
		Attack,	//攻撃
		Dodge	//回避
	};

	Player();
	virtual~Player();

	void Init()override;
	void Update()override{};
	void Update(Input&input);
	void Draw()override;

	//被ダメージ時の処理
	void OnHit(GameObject* attacker)override;

	//移動処理
	void Move(Input& input);
	//攻撃処理
	void StartAttack();
	//回避処理
	void StartDodge();

	//各種タイマーの更新処理
	void UpdateTimers();
	//入力受付(攻撃や回避の開始など)
	void HandleInput(Input&input);
	//状態に応じた処理の分岐を行う
	void UpdateAction(Input&input);
	//状態遷移の処理
	void UpdateState();
	//攻撃中の処理
	void UpdateAttack();
	//回避中の処理
	void UpdateDodge();
	//アニメーションの更新
	void UpdateAnimation();
	//当たり判定の更新
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
	//攻撃判定取得
	SphereCollider* GetAttackCollider() { return &attackCollider_; }

	//衝突処理
	void OnCollision(GameObject* other);
	//無敵状態かどうか
	bool IsInvincible()const { return invincibleTimer_ > 0.0f; }
	//ジャスト回避の状態かどうか
	bool IsJustDodge()const { return isJustDodge_; }

private:
	float cameraAngle_;						//カメラ用の角度
	float moveAngle_;						//プレイヤーの移動用の角度
	bool isHit_;							//衝突判定用フラグ
	float attackTimer_;						//攻撃時間
	float dodgeTimer_;						//回避時間
	float invincibleTimer_;					//無敵時間
	float afterImageTimer_;					//残像の生成時間
	float justDodgeTimer_;					//ジャスト回避の時間
	bool isJustDodge_;						//ジャスト回避のフラグ

	SphereCollider collider_;				//当たり判定用変数
	SphereCollider attackCollider_;			//攻撃の当たり判定用変数
	Animation animation_;					//アニメーション
	Model model_;							//モデル
	Model ghostModel_;						//残像用のモデル
	PlayerState state_;						//プレイヤーの状態
	std::vector<AfterImage>afterImages_;	//残像の情報
};

