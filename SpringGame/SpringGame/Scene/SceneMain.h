#pragma once
#include<memory>
#include "Scene.h"
#include"../Physics/CollisionManager.h"
#include"../UIManager.h"
#include"../Bg.h"

class Enemy;
class Player;
class Input;
class Camera;
class PopUI;
class SceneMain :public Scene
{
public:
	SceneMain(SceneController& contorller);
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();

	void AddScorePop(const Vector3& pos, int value);
	void DrawCenterTextWithOutline(const char* text, int y, int color, int screenW = 1280);

private:
	void FadeInUpdate(Input& input);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input& input);
	using UpdateFunc_t = void (SceneMain::*)(Input&);
	UpdateFunc_t update_;//update系を受け取るメンバ関数ポインタ
	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (SceneMain::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ

private:
	int frameCount_;				//フレームカウント
	int hpHandle_;					//HPUIのハンドル
	int floorHandle_;				//床のモデル
	int shadowMapHandle_ = -1;		//シャドウマップハンドル
	float playTime_;				//制限時間
	float bonusTime_;				//ボーナスタイム
	float dt_;						//経過時間
	float timeScale_;				//時間のスケール(スロー演出などで使用)
	float slowTimer_;				//スロー時間のタイマー
	float timeBonusDisplay_ = 0.0f;	//時間ボーナスの表示用タイマー
	float timeBonusTimer_ = 0.0f;	//時間ボーナスのタイマー
	bool isClearing_ = false;		//クリア中かどうか
	bool fadeFinished_ = false;		//フェードが終了したかどうか
	int prevHp_ = 0;				//前フレームのHP
	int displayHp_ = 0;				//表示しているHP
	int hpAnimFrame_ = 0;			//HPアニメーションのフレーム
	int hpKeepFrame_ = 0;			//HP保持フレーム
	float hpAnimTimer_ = 0.0f;		//HPアニメーションのタイマー
	bool isHpAnimating_ = false;	//HPアニメーション中かどうか
	int damageIndex_ = -1;			//ダメージインデックス

	Bg bg_;
	std::vector<std::shared_ptr<Enemy>> enemies_;
	std::vector<PopUI>pPopUIs_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	UIManager uiManager_;
	CollisionManager collisionManager_;
};

