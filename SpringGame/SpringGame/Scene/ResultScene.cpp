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
	frameCount_(kFadeInterval),
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

	if (frameCount_ >= kFadeInterval)
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

	//選択を選ぶ
	if (input.IsTriggered("up") || input.IsTriggered("down"))
	{
		if (currentMenu_ == ResultMenu::Retry)
		{
			currentMenu_ = ResultMenu::Title;
		}
		else
		{
			currentMenu_ = ResultMenu::Retry;
		}
	}

	//リトライが行われたら
	if (input.IsTriggered("retry"))
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);

		update_ = &ClearScene::FadeOutUpdate;
		draw_ = &ClearScene::FadeDraw;

		frameCount_ = kFadeInterval;
	}
}

void ClearScene::FadeOutUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		if (currentMenu_ == ResultMenu::Retry)
		{
			controller_.ResetScene(std::make_shared<GameScene>(controller_));
		}
		else
		{
			controller_.ResetScene(std::make_shared<TitleScene>(controller_));
		}
	}
}

void ClearScene::FadeDraw()
{
	//通常の描画
	NormalDraw();

	float rate;

	if (update_ == &ClearScene::FadeInUpdate)
	{
		// フェードイン
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}
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

	const char* retryText = "リトライ";
	const char* titleText = "タイトルへもどる";

	int centerX = Game::kScreenWidth / 2;
	int baseY = Game::kScreenHeight / 2 + 50;

	//拡縮
	float retryScale = (currentMenu_ == ResultMenu::Retry) ? 1.0f + 0.1f * sinf(blinkTimer_ * 5.0f) : 1.0f;
	float titleScale = (currentMenu_ == ResultMenu::Title) ? 1.0f + 0.1f * sinf(blinkTimer_ * 5.0f) : 1.0f;

	//文字幅取得
	int retryW = GetDrawStringWidthToHandle(retryText, strlen(retryText), Game::kFontUIHandle);
	int titleW = GetDrawStringWidthToHandle(titleText, strlen(titleText), Game::kFontUIHandle);

	//描画
	DrawExtendStringToHandle(
		centerX - retryW / 2,
		baseY,
		retryScale,
		retryScale,
		retryText,
		GetColor(255, 255, 255),
		Game::kFontUIHandle);

	DrawExtendStringToHandle(
		centerX - titleW / 2,
		baseY + 50,
		titleScale,
		titleScale,
		titleText,
		GetColor(255, 255, 255),
		Game::kFontUIHandle);
}