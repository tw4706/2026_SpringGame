#include "ClearScene.h"
#include"SceneController.h"
#include"../ScoreManager.h"
#include"SceneMain.h"
#include"../Input.h"
#include"../Game.h"
#include<algorithm>
#include<Dxlib.h>

namespace
{
	constexpr int kFadeInterval = 60;
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
	resultScore_ = ScoreManager::GetScore();
	displayScore_ = 0;

	SetFontSize(40);
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

	if (input.IsTriggered("retry"))
	{
		ScoreManager::Reset();
		controller_.ResetScene(std::make_shared<SceneMain>(controller_));
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
	// 通常描画（UIなど）
	NormalDraw();

	float rate = (float)frameCount_ / kFadeInterval;
	rate = std::clamp(rate, 0.0f, 1.0f);

	// 黒 → 透明
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::NormalDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// スコア表示
	char scoreText[64];
	sprintf_s(scoreText, "SCORE: %d", displayScore_);

	int scoreW = GetDrawStringWidth(scoreText, strlen(scoreText));
	int scoreX = (Game::kScreenWidth - scoreW) / 2;

	DrawStringToHandle(scoreX, Game::kScreenHeight / 2-100,
		scoreText, GetColor(255, 255, 0), Game::kFontHandle);

	//リトライ表示
	const char* retryText = "Press Retry";
	int retryW = GetDrawStringWidth(retryText, strlen(retryText));
	int retryX = (Game::kScreenWidth - retryW) / 2;

	float alphaRate = (sinf(blinkTimer_) + 1.0f) * 0.5f;
	int alpha = (int)(255 * alphaRate);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringToHandle(retryX, Game::kScreenHeight / 2, 
		retryText, GetColor(255, 255, 255), Game::kFontHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}