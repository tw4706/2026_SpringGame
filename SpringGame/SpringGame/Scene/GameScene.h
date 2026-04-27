#pragma once
#include<memory>
#include "Scene.h"
#include"../Manager/CollisionManager.h"
#include"../UI/GameSceneUI.h"
#include"../GameObject/Bg.h"

class Enemy;
class Player;
class Input;
class Camera;
class PopUI;
class GoalObject;
class EnemySpawner;
class OperationGuideUI;
class GameScene :public Scene
{
public:
	GameScene(SceneController& contorller);
	~GameScene();

	void Init();
	void Update(Input& input);
	void Draw();

	/// <summary>
	/// ポップスコアの加算
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="value">何ポイントか</param>
	void AddScorePop(const Vector3& pos, int value);

	/// <summary>
	/// 描画する文字列を画面中央に描画する関数
	/// </summary>
	/// <param name="text">文字列</param>
	/// <param name="y">y座標</param>
	/// <param name="color">カラー</param>
	/// <param name="screenW">画面の幅</param>
	void DrawCenterTextWithOutline(const char* text, int y, int color, int screenW = 1280);

private:
	void FadeInUpdate(Input& input);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input& input);
	using UpdateFunc_t = void (GameScene::*)(Input&);
	UpdateFunc_t update_;//Update系を受け取るメンバ関数ポインタ
	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (GameScene::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ

private:
	int frameCount_ = 0;											//フレームカウント
	int hpHandle_ = -1;												//HPUIのハンドル
	int floorHandle_ = -1;											//床のモデルハンドル
	int shadowMapHandle_ = -1;										//シャドウマップハンドル
	int damageIndex_ = -1;											//ダメージインデックス
	int prevHp_ = 0;												//前フレームのHP
	int displayHp_ = 0;												//表示しているHP
	int hpAnimFrame_ = 0;											//HPUIアニメーションのフレーム
	int hpKeepFrame_ = 0;											//HPUI保持フレーム
	float clearTime_ = 0.0f;										//残り時間
	float dt_ = 0.0f;												//経過時間
	float timeScale_ = 0.0f;										//時間のスケール(スロー演出などで使用)
	float slowTimer_ = 0.0f;										//スロー時間のタイマー
	float hpAnimTimer_ = 0.0f;										//HPUIアニメーションのタイマー
	float gameStartTimer_ = 0.0f;									//ゲーム開始合図のタイマー
	bool isHpAnimating_ = false;									//HPUIがアニメーション中かどうか
	bool isClearing_ = false;										//クリア中かどうか
	bool fadeFinished_ = false;										//フェードが終了したかどうか
	bool isGameStarted_ = false;									//ゲーム開始したかどうか
	int currentWave_ = 0;											//現在のウェーブ数

	Bg bg_;															//背景
	GameSceneUI	gameSceneUI_;										//ゲームシーン用UI
	std::shared_ptr<OperationGuideUI>pOperationGuideUI_;			//操作説明
	CollisionManager collisionManager_;								//当たり判定マネージャー
	std::vector<bool> prevLocked_;									//スポナーを壊したかどうか
	std::vector<PopUI>pPopUIs_;										//プレイヤーの頭上に表示するPopUI
	std::shared_ptr<Player>pPlayer_;								//プレイヤー
	std::shared_ptr<Camera>pCamera_;								//カメラ
	std::shared_ptr<GoalObject>pGoalObject_;						//ゴール
	std::vector<std::shared_ptr<EnemySpawner>>pEnemySpawner_;		//敵のスポナー(生成場所)
};

