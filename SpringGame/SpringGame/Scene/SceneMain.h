#pragma once
#include<memory>
#include "Scene.h"
#include"../Physics/CollisionManager.h"

class Enemy;
class Player;
class Input;
class Camera;
class ScorePop;
class SceneMain :public Scene
{
public:
	SceneMain(SceneController& contorller);
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();

	void AddScorePop(const Vector3& pos, int value);

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
	int skyTexture_[6];
	int shadowMap_;

	std::vector<std::shared_ptr<Enemy>> enemies_;
	std::vector<ScorePop>pScorePops_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	CollisionManager collisionManager_;
};

