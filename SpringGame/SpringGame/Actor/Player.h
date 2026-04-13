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

	/// <summary>
	/// 被ダメージ処理
	/// </summary>
	/// <param name="attacker">攻撃してきたゲームオブジェクト</param>
	void OnHit(GameObject* attacker)override;

	/// <summary>
	/// プレイヤーの座標の取得
	/// </summary>
	/// <returns>座標</returns>
	Vector3 GetPos()const{ return pos_; }

	/// <summary>
	/// カメラの中止店の取得
	/// </summary>
	/// <returns>カメラの注視点位置</returns>
	Vector3 GetCameraTarget()const;

	/// <summary>
	/// HPの取得
	/// </summary>
	/// <returns>HP</returns>
	int GetHP() const { return hp_; }

	/// <summary>
	/// MaxHPの取得
	/// </summary>
	/// <returns>MaxHP</returns>
	int GetMaxHP()const;

	/// <summary>
	/// 当たり判定の取得
	/// </summary>
	/// <returns>当たり判定の参照</returns>
	SphereCollider* GetCollider() { return &collider_; }

	/// <summary>
	/// 攻撃の当たり判定の取得
	/// </summary>
	/// <returns>攻撃判定のコライダーの参照</returns>
	SphereCollider* GetAttackCollider() { return &attackCollider_; }

	//カメラのセット
	void SetCamera(Camera* camera) { pCamera_ = camera; }

	//衝突処理
	void OnCollision(GameObject* other);
	//無敵状態かどうか
	bool IsInvincible()const { return invincibleTimer_ > 0.0f; }
	//ジャスト回避の状態かどうか
	bool IsJustDodge();
	bool IsJustDodge()const { return isJustDodge_; }
	bool IsJustDodgeTriggered()const { return isJustDodgeTriggered_; }
	bool IsDead() const { return hp_ <= 0; }

	bool IsDeathAnimEnd()const { return state_ == PlayerState::Death && animation_.IsEnd(); }
private:

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="input">入力</param>
	/// <param name="dt">経過時間</param>
	void Move(Input& input, float dt);

	/// <summary>
	/// 攻撃開始処理
	/// </summary>
	void StartAttack();

	/// <summary>
	/// 回避開始処理
	/// </summary>
	void StartDodge();

	/// <summary>
	/// 各タイマーの更新処理
	/// </summary>
	/// <param name="deltaTime">経過時間</param>
	void UpdateTimers(float deltaTime);


	/// <summary>
	/// 入力受付(攻撃や回避の開始など)
	/// </summary>
	/// <param name="input"></param>
	void HandleInput(Input& input);


	/// <summary>
	/// 状態に応じた処理の分岐
	/// </summary>
	/// <param name="input">入力</param>
	/// <param name="dt">経過時間</param>
	void UpdateAction(Input& input, float dt);

	/// <summary>
	/// 状態遷移処理
	/// </summary>
	void UpdateState();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// ノックバック処理
	/// </summary>
	/// <param name="dt">経過時間</param>
	void UpdateKnockBack(float dt);


	/// <summary>
	/// 回避処理
	/// </summary>
	/// <param name="dt">経過時間</param>
	void UpdateDodge(float dt);

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="dt">経過時間</param>
	void UpdateAnimation(float dt);

	/// <summary>
	/// 当たり判定の更新
	/// </summary>
	void UpdateCollision();

	/// <summary>
	/// アナログスティックの更新処理
	/// </summary>
	/// <param name="input">入力</param>
	void UpdateAnalogStick(Input& input);

	/// <summary>
	/// 行列の更新処理
	/// </summary>
	void UpdateMatrix();
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
	int hp_;								//HP
	Vector3 knockbackVel_;					//ノックバック速度
	float knockbackTimer_;					//ノックバックタイマー
	bool isTouchingWall_;					//壁に触れているかのフラグ

	SphereCollider collider_;				//当たり判定用変数
	SphereCollider attackCollider_;			//攻撃の当たり判定用変数
	Animation animation_;					//アニメーション
	Model model_;							//モデル
	Model ghostModel_;						//残像用のモデル
	PlayerState state_;						//プレイヤーの状態
	Camera* pCamera_ = nullptr;				//カメラ
	std::vector<AfterImage>afterImages_;	//残像の情報
};

