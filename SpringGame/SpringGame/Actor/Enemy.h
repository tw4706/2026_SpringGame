#pragma once
#include"../Model.h"
#include"../Animation.h"
#include "../GameObject.h"
#include "../Physics/SphereCollider.h"

class Player;
class Camera;
class GameScene;
class Enemy :public GameObject
{
public:
	Enemy();
	virtual~Enemy();

	void Init()override;
	void Update()override {};
	void Update(float dt);
	void Draw()override;

	/// <summary>
	/// 被ダメージ処理
	/// </summary>
	/// <param name="attacker">攻撃してきたゲームオブジェクト</param>
	void OnHit(GameObject* attacker)override;

	/// <summary>
	/// スフィアコライダーの当たり判定の取得
	/// </summary>
	/// <returns>当たり判定の参照</returns>
	SphereCollider* GetCollider() { return &collider_; }

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	Vector3 GetPos()const { return pos_; }

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other">当たり判定を行うゲームオブジェクト</param>
	void OnCollision(GameObject* other);

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	void SetPlayer(Player* player) { pPlayer_ = player; }

	/// <summary>
	/// 座標のセット
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPos(const Vector3& pos) { pos_ = pos; }

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラのポインタ</param>
	void SetCamera(Camera* camera) { pCamera_ = camera; }

	/// <summary>
	/// シーンのセット
	/// </summary>
	/// <param name="scene">シーン名</param>
	void SetScene(GameScene* scene) { pScene_ = scene; }

	/// <summary>
	/// 敵が削除されているかどうか
	/// </summary>
	/// <returns>削除されていたらtrue,削除されていなかったらfalse</returns>
	bool IsDestroy()const { return isDestroy_; }
private:

	/// <summary>
	/// アニメーションの状態の取得
	/// </summary>
	/// <returns>アニメーション状態</returns>
	AnimationState GetState()const;

private:
	SphereCollider collider_;		//スフィアコライダー
	bool isHit_;					//攻撃当たっているかどうか
	float hitTimer_;				//ヒットタイマー
	bool isDead_;					//死亡しているかどうか
	bool isDestroy_;				//削除されているかどうか
	bool isSpawning_;				//生成されているかどうか

	Player* pPlayer_=nullptr;		//プレイヤーのポインタ
	Camera* pCamera_ = nullptr;		//カメラのポインタ
	GameScene* pScene_ = nullptr;	//ゲームシーンのポインタ
	Model model_;					//モデル
	Model copyModel_;				//複製用のモデル
	Animation animation_;			//アニメーション
	AnimationState state_;			//アニメーションの状態
};

