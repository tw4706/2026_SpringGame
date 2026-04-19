#include "ResultScene.h"
#include "TitleScene.h"
#include"SceneController.h"
#include"GameScene.h"
#include"../System/Input.h"
#include"../Game.h"
#include"../System/Application.h"
#include<algorithm>
#include<Dxlib.h>

namespace
{
	constexpr int kFadeInterval = 60;

	constexpr int kscoreTextOffsetY = 100;
}

ClearScene::ClearScene(SceneController& controller) :
	Scene(controller),
	update_(&ClearScene::FadeInUpdate),
	draw_(&ClearScene::FadeDraw),
	frameCount_(0),
	resultScore_(0),
	displayScore_(0)
{
	frameCount_ = kFadeInterval;

	//BGM再生
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Result);
}

ClearScene::~ClearScene()
{
}

void ClearScene::Update(Input& input)
{
	(this->*update_)(input);
}
void ClearScene::Draw()
{
	(this->*draw_)();
}

void ClearScene::FadeInUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		update_ = &ClearScene::NormalUpdate;
		draw_ = &ClearScene::NormalDraw;
	}
}

void ClearScene::NormalUpdate(Input& input)
{
	frameCount_++;
	blinkTimer_ += 0.1f;

	if (frameCount_ >= 60)
	{
		if (displayScore_ < resultScore_)
		{
			int diff = resultScore_ - displayScore_;
			displayScore_ += (std::max)(1, diff / 10);

			if (displayScore_ > resultScore_)
			{
				displayScore_ = resultScore_;
			}
		}
	}

	//リトライが行われたら
	if (input.IsTriggered("retry"))
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);
		
		//ゲームシーンに戻る
		controller_.ResetScene(std::make_shared<GameScene>(controller_));
		return;
	}
}

void ClearScene::FadeOutUpdate(Input& input)
{
	if (frameCount_++ <= 0)
	{
		update_ = &ClearScene::FadeInUpdate;
	}
}

void ClearScene::FadeDraw()
{
	//通常の描画
	NormalDraw();

	float rate = (float)frameCount_ / kFadeInterval;
	rate = std::clamp(rate, 0.0f, 1.0f);

	//真っ黒画面から透明
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::NormalDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//リトライ表示
	const char* retryText = "Aボタンでリスタート";
	int retryW = GetDrawStringWidthToHandle(retryText, static_cast<int>(strlen(retryText)), Game::kFontUIHandle);
	int retryX = (Game::kScreenWidth - retryW) / 2;

	//透明度
	float alphaRate = (sinf(blinkTimer_) + 1.0f) * 0.5f;
	int alpha = (int)(255 * alphaRate);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringToHandle(retryX, Game::kScreenHeight / 2 + 150, retryText, GetColor(255, 255, 255), Game::kFontUIHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}