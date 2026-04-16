#pragma once
#include "Scene.h"
#include"../Bg.h"
#include"../Actor/Player.h"

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
	int frameCount_;
	float bgAngle_;
	float blinkTimer_;

	Bg bg_;				//îwåi
};

