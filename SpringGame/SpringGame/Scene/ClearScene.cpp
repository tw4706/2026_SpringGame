#include "ClearScene.h"
#include"SceneController.h"
#include"../ScoreManager.h"
#include"SceneMain.h"
#include"../Input.h"
#include<algorithm>
#include<Dxlib.h>

namespace
{
	constexpr int kFadeInterval = 60;
}

ClearScene::ClearScene(SceneController& controller):
	Scene(controller),
	update_(&ClearScene::FadeInUpdate),
	draw_(&ClearScene::FadeDraw),
	frameCount_(0)
{
	frameCount_ = kFadeInterval;
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
	mode_ = FadeMode::In;
	if (frameCount_-- <= 0)
	{
		update_ = &ClearScene::NormalUpdate;
		draw_ = &ClearScene::NormalDraw;
	}
}

void ClearScene::NormalUpdate(Input& input)
{
	if (input.IsTriggered("retry"))
	{
		ScoreManager::Reset();
		controller_.ChangeScene(std::make_shared<SceneMain>(controller_));
	}
}

void ClearScene::FadeOutUpdate(Input& input)
{
	mode_ = FadeMode::Out;
	if (frameCount_++ <= 0)
	{
		update_ = &ClearScene::FadeInUpdate;
		draw_ = &ClearScene::FadeDraw;
	}
}

void ClearScene::FadeDraw()
{
	NormalDraw();
	float rate = 0.0f;

	if (mode_ == FadeMode::In)
	{
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}

	rate = std::clamp(rate, 0.0f, 1.0f);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * rate));
	DrawBoxAA(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ClearScene::NormalDraw()
{
	DrawString(500, 250, "CLEAR!", GetColor(255, 255, 255));

	DrawFormatString(500, 300, GetColor(255, 255, 0),
		"SCORE: %d", ScoreManager::GetScore());
}