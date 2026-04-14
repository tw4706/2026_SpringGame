#include "TitleScene.h"
#include"../Physics/Vector3.h"
#include "SceneController.h"
#include "SceneMain.h"
#include"../Input.h"
#include"../Game.h"
#include"../Application.h"
#include<Dxlib.h>
#include<memory>
#include<algorithm>

namespace
{
	constexpr int kFadeInterval = 60;
}

TitleScene::TitleScene(SceneController&controller):
	Scene(controller),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval),
	bgAngle_(0.0f)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	bg_.Init();
}

void TitleScene::Update(Input&input)
{
	(this->*update_)(input);
}

void TitleScene::Draw()
{
	(this->*draw_)();
}

void TitleScene::FadeInUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		update_ = &TitleScene::NormalUpdate;
		draw_ = &TitleScene::NormalDraw;
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	bgAngle_ += 0.003f;

	if (input.IsTriggered("retry"))
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		frameCount_ = 0;
	}
}

void TitleScene::FadeOutUpdate(Input& input)
{
	bgAngle_ += 0.003f;
	if (frameCount_++ >= kFadeInterval)
	{
		controller_.ChangeScene(std::make_shared<SceneMain>(controller_));
	}
}

void TitleScene::FadeDraw()
{
	NormalDraw();

	float rate = frameCount_ / 60.0f;
	rate = std::clamp(rate, 0.0f, 1.0f);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight,
		GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::NormalDraw()
{
	Vector3 center(0.0f, 0.0f, 0.0f);

	Vector3 cameraPos;
	cameraPos.x_ = cosf(bgAngle_) * 500.0f;
	cameraPos.y_ = -100.0f;
	cameraPos.z_ = sinf(bgAngle_) * 500.0f;

	// カメラを中心へ向ける
	SetCameraPositionAndTarget_UpVecY(
		cameraPos.ToDxlibVector(),
		center.ToDxlibVector());

	bg_.Draw(center);

	const char* text = "Title Scene";

	//文字の横幅を取得
	int width = GetDrawStringWidthToHandle(text,static_cast<int>(strlen(text)),Game::kTitleFontHandle);

	//画面サイズの半分(真ん中)
	int x = (Game::kScreenWidth - width) / 2;
	int y = Game::kScreenHeight / 2-100;

	//描画　
	DrawStringToHandle(x + 4, y + 4, text, 0x000000, Game::kTitleFontHandle);
	DrawStringToHandle(x, y, text, 0xffff00, Game::kTitleFontHandle);
}


