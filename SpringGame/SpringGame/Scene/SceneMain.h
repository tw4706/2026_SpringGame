#pragma once
#include<memory>
#include "Scene.h"
#include"../Physics/CollisionManager.h"
#include"../UIManager.h"

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
	
	void DrawGrid();
	void DrawSkyQuad(VECTOR a, VECTOR b, VECTOR c, VECTOR d, int tex);
	void DrawSkybox();

private:
	int frameCount_;
	float playTime_;//制限時間
	float bonusTime_;
	int skyTexture_[6];
	float dt_;
	float timeScale_;//時間のスケール(スロー演出などで使用)
	float slowTimer_;
	float timeBonusDisplay_ = 0.0f;
	float timeBonusTimer_ = 0.0f;

	std::vector<std::shared_ptr<Enemy>> enemies_;
	std::vector<PopUI>pPopUIs_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	UIManager uiManager_;
	CollisionManager collisionManager_;
};

