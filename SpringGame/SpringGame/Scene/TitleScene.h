#pragma once
#include "Scene.h"
#include"../GameObject/Bg.h"
#include"../GameObject/TitlePlayer.h"
#include"../GameObject/TitleEnemy.h"
#include"../GameObject/Model.h"
#include<memory>

class TitleScene :public Scene
{
public:
	TitleScene(SceneController&controller);
	~TitleScene();

	void Init()override;
	void Update(Input&input);
	void Draw();

	void FadeInUpdate(Input& input);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input& input);
	using UpdateFunc_t = void (TitleScene::*)(Input&);
	UpdateFunc_t update_;

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (TitleScene::*)();
	DrawFunc_t draw_;

private:
	int frameCount_ = 0;
	float bgAngle_ = 0.0f;
	float blinkTimer_ = 0.0f;
	int titleHandle_ = -1;

	Bg bg_;											//背景
	Model floorModel_;								//床のモデル
	std::shared_ptr<TitlePlayer>pTitlePlayer_;		//タイトル演出用プレイヤー
	std::shared_ptr<TitleEnemy>pTitleEnemy_;		//タイトル演出用エネミー
};

