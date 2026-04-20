#include "TitleScene.h"
#include"../Physics/Vector3.h"
#include "SceneController.h"
#include "GameScene.h"
#include"../System/Input.h"
#include"../Game.h"
#include"../System/Application.h"
#include<Dxlib.h>
#include<memory>
#include<algorithm>

namespace
{
	constexpr int kFadeInterval = 60;

	constexpr int kBlinkBaseAlpha = 150;
	constexpr int kBlinkAlphaRange = 120;
	constexpr float kBlinkSpeed = 0.05f;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval),
	bgAngle_(0.0f),
	blinkTimer_(0.0f),
	titleHandle_(-1)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	bg_.Init();

	SetCameraNearFar(1.0f, 10000.0f);

	//カメラ設定
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, -500),VGet(0, 0, 0));

	titleHandle_ = LoadGraph("data/titleLogo.png");

	update_ = &TitleScene::FadeInUpdate;
	draw_ = &TitleScene::FadeDraw;

	frameCount_ = kFadeInterval;

	bgAngle_ = 0.0f;
	blinkTimer_ = 0.0f;


	//BGM再生
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Title);
}

void TitleScene::Update(Input& input)
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

	blinkTimer_++;

	if (input.IsTriggered("retry"))
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		frameCount_ = kFadeInterval;
	}
}

void TitleScene::FadeOutUpdate(Input& input)
{
	bgAngle_ += 0.003f;
	if (frameCount_-- <= 0)
	{
		controller_.ChangeScene(std::make_shared<GameScene>(controller_));
	}
}

void TitleScene::FadeDraw()
{
	NormalDraw();

	float rate;

	if (update_ == &TitleScene::FadeInUpdate)
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

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight,GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::NormalDraw()
{
	Vector3 center(0.0f, 0.0f, 0.0f);

	Vector3 cameraPos;
	cameraPos.x_ = cosf(bgAngle_) * 500.0f;
	cameraPos.y_ = 0.0f;
	cameraPos.z_ = sinf(bgAngle_) * 500.0f;

	//カメラを画面中央へ向ける
	SetCameraPositionAndTarget_UpVecY(
		cameraPos.ToDxlibVector(),
		center.ToDxlibVector());

	SetUseBackCulling(FALSE);
	//描画
	bg_.Draw(cameraPos);
	SetUseBackCulling(TRUE);

	//タイトル名
	const char* pressStartText = "ボタンをおしてスタート";

	////文字の横幅を取得
	int pressStartWidth = GetDrawStringWidthToHandle(pressStartText, static_cast<int>(strlen(pressStartText)), Game::kTitleFontHandle);

	//描画
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 0.1f, 0.0f, titleHandle_, true);
	;
	int alpha = static_cast<int>(kBlinkBaseAlpha + kBlinkAlphaRange * sinf(blinkTimer_ * kBlinkSpeed));

	int pressStartx = (Game::kScreenWidth - pressStartWidth) / 2;
	int pressStarty = Game::kScreenHeight / 2 + 100;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringToHandle(pressStartx + 4, pressStarty + 4, pressStartText, 0x000000, Game::kTitleFontHandle);
	DrawStringToHandle(pressStartx, pressStarty, pressStartText, 0xffffff, Game::kTitleFontHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}