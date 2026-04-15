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

	constexpr int kBlinkBaseAlpha = 128;
	constexpr int kBlinkAlphaRange = 127;
	constexpr float kBlinkSpeed = 0.05f;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval),
	bgAngle_(0.0f),
	blinkTimer_(0.0f)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	bg_.Init();
	titlePlayer_.Init();
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Title);

	fallingPlayers_.clear();
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
		frameCount_ = 0;
	}

	//ボタンを押すとタイトル用プレイヤーが生成される
	if (input.IsTriggered("fallPlayer"))
	{
		float x = static_cast<float>(GetRand(600) - 300);
		float z = static_cast<float>(GetRand(600) - 300);

		fallingPlayers_.push_back({{x, 300.0f, z},0.0f,0.0f});

		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);
	}

	//タイトル用プレイヤーの更新
	for (auto& player : fallingPlayers_)
	{
		player.speed += 0.4f;
		player.pos.y_ -= player.speed;
		player.rotY += 0.2f;
	}
	
	//落下するタイトル用プレイヤーの削除処理
	fallingPlayers_.erase(std::remove_if(fallingPlayers_.begin(),fallingPlayers_.end(),
			[](const FallingPlayer& player){return player.pos.y_ < -500.0f;}),fallingPlayers_.end());
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
	cameraPos.y_ = 0.0f;
	cameraPos.z_ = sinf(bgAngle_) * 500.0f;

	// カメラを中心へ向ける
	SetCameraPositionAndTarget_UpVecY(
		cameraPos.ToDxlibVector(),
		center.ToDxlibVector());

	//背景の描画
	bg_.Draw(center);

	//タイトル用プレイヤーの描画
	for (auto& player : fallingPlayers_)
	{
		titlePlayer_.DrawTitlePlayer(player.pos, player.rotY);
	}

	//タイトル名
	const char* text = "らびっとスマッシュ";
	const char* pressStartText = "ボタンをおしてスタート";

	//文字の横幅を取得
	int titleWidth = GetDrawStringWidthToHandle(text, static_cast<int>(strlen(text)), Game::kTitleFontHandle);

	int pressStartWidth = GetDrawStringWidthToHandle(pressStartText, static_cast<int>(strlen(pressStartText)), Game::kTitleFontHandle);

	//画面サイズの半分(真ん中)
	int x = (Game::kScreenWidth - titleWidth) / 2;
	int y = Game::kScreenHeight / 2 - 200;

	//描画　
	DrawStringToHandle(x + 4, y + 4, text, 0x000000, Game::kTitleFontHandle);
	DrawStringToHandle(x, y, text, 0xffff00, Game::kTitleFontHandle);

	int alpha = static_cast<int>(kBlinkBaseAlpha + kBlinkAlphaRange * sinf(blinkTimer_ * kBlinkSpeed));

	int pressStartx = (Game::kScreenWidth - pressStartWidth) / 2;
	int pressStarty = Game::kScreenHeight / 2 + 100;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringToHandle(pressStartx + 4, pressStarty + 4, pressStartText, 0x000000, Game::kTitleFontHandle);
	DrawStringToHandle(pressStartx, pressStarty, pressStartText, 0xffffff, Game::kTitleFontHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


