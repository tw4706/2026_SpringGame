#pragma once
#include "Scene.h"

class SceneController;
class Input;
class ClearScene :public Scene
{
public:
	ClearScene(SceneController& controller);
	~ClearScene();

	void Init()override {};
	void Update(Input& input);
	void Draw();

private:
	void FadeInUpdate(Input& input);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input& input);
	using UpdateFunc_t = void (ClearScene::*)(Input&);
	UpdateFunc_t update_;//update系を受け取るメンバ関数ポインタ

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (ClearScene::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ
private:
	int frameCount_;
};

