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
class Camera;
class Player :public GameObject
{
public:
	//プレイヤーの状態
	enum class PlayerState
	{
		Idle,	//待機
		Run,	//移動
		Attack,	//攻撃
		Dodge,	//回避
		Hit,	//ダメージ
		Death	//死亡
	};

	Player();
	virtual~Player();

	void Init()override;
	void Update()override{};
	void Update(Input&input, float dt);
	void Draw()override;

	//被ダメージ時の処理
	void OnHit(GameObject* attacker)override;

	//移動処理
	void Move(Input& input, float dt);
	//攻撃処理
	void StartAttack();
	//回避処理
	void StartDodge();

	//各種タイマーの更新処理
	void UpdateTimers(float deltaTime);
	//入力受付(攻撃や回避の開始など)
	void HandleInput(Input&input);
	//状態に応じた処理の分岐を行う
	void UpdateAction(Input&input,float dt);
	//状態遷移の処理
	void UpdateState();
	//攻撃処理
	void UpdateAttack();
	//ノックバック処理
	void UpdateKnockBack(float dt);
	//回避処理
	void UpdateDodge(float dt);
	//アニメーションの更新
	void UpdateAnimation(float dt);
	//当たり判定の更新
	void UpdateCollision();
	//アナログスティックの更新処理
	void UpdateAnalogStick(Input& input);
	//行列の更新処理
	void UpdateMatrix();

	//プレイヤーの位置を取得
	Vector3 GetPos()const{ return pos_; }
	//カメラの注視点の取得
	Vector3 GetCameraTarget()const;
	//HPの取得
	int GetHP() const { return hp_; }
	int GetMaxHP()const;

	//当たり判定の取得
	SphereCollider* GetCollider() { return &collider_; }
	//攻撃判定取得
	SphereCollider* GetAttackCollider() { return &attackCollider_; }

	//カメラのセット
	void SetCamera(Camera* camera) { pCamera_ = camera; }

	//衝突処理
	void OnCollision(GameObject* other);
	//無敵状態かどうか
	bool IsInvincible()const { return invincibleTimer_ > 0.0f; }
	//ジャスト回避の状態かどうか
	bool ConsumeJustDodge();
	bool IsJustDodge()const { return isJustDodge_; }
	bool IsJustDodgeTriggered()const { return isJustDodgeTriggered_; }
	bool IsDead() const { return hp_ <= 0; }

	bool IsDeathAnimEnd()const { return state_ == PlayerState::Death && animation_.IsEnd(); }

private:
	float moveAngle_;						//プレイヤーの移動用の角度
	bool isHit_;							//衝突判定用フラグ
	float attackTimer_;						//攻撃時間
	float dodgeTimer_;						//回避時間
	float hitTimer_;						//被ダメージ時間
	float invincibleTimer_;					//無敵時間
	float afterImageTimer_;					//残像の生成時間
	bool isJustDodge_;						//ジャスト回避のフラグ
	bool isJustDodgeTriggered_;				//ジャスト回避の判定がされたかどうか
	int justDodgeFrame_;					//ジャスト回避の猶予フレーム数
	int hp_;
	Vector3 knockbackVel_;
	float knockbackTimer_;
	bool isTouchingWall_;

	SphereCollider collider_;				//当たり判定用変数
	SphereCollider attackCollider_;			//攻撃の当たり判定用変数
	Animation animation_;					//アニメーション
	Model model_;							//モデル
	Model ghostModel_;						//残像用のモデル
	PlayerState state_;						//プレイヤーの状態
	Camera* pCamera_ = nullptr;
	std::vector<AfterImage>afterImages_;	//残像の情報
};

