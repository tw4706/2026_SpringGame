#pragma once
#include "Scene.h"
#include"../GameObject/Bg.h"

enum class ResultTextType
{
	Clear,
	GameOver
};

class SceneController;
class Input;
class ResultScene :public Scene
{
public:
	ResultScene(SceneController& controller,float clearTime,ResultTextType resultType);
	~ResultScene();

	void Init()override;
	void Update(Input& input);
	void Draw();

private:
	void FadeInUpdate(Input& input);
	void NormalUpdate(Input& input);
	void FadeOutUpdate(Input& input);
	using UpdateFunc_t = void (ResultScene::*)(Input&);
	UpdateFunc_t update_;//update系を受け取るメンバ関数ポインタ

	void FadeDraw();
	void NormalDraw();
	using DrawFunc_t = void (ResultScene::*)();
	DrawFunc_t draw_;//Draw系を受け取るメンバ関数ポインタ
private:
	enum class ResultMenu
	{
		Retry,
		Title
	};
private:
	int frameCount_;
	float blinkTimer_ = 0.0f;
	int resultScore_ = 0;
	int displayScore_ = 0;
	float clearTime_;	//クリア時間
	float bgAngle_;

	Bg bg_;
	ResultMenu currentMenu_=ResultMenu::Retry;	//現在選択されている選択肢の状態
	ResultTextType resultType_;					//リザルトで表示するテキストのタイプ
};