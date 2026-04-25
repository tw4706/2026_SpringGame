#include "TitleScene.h"
#include"../Physics/Vector3.h"
#include "SceneController.h"
#include "GameScene.h"
#include"../System/Input.h"
#include"../Game.h"
#include"../System/Application.h"
#include"../GameObject/TitlePlayer.h"
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
	pTitlePlayer_ = std::make_shared<TitlePlayer>();
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	bg_.Init();

	pTitlePlayer_->Init();

	SetCameraNearFar(1.0f, 10000.0f);

	//カメラ設定
	SetCameraPositionAndTarget_UpVecY(VGet(0, 0, -500), VGet(0, 0, 0));

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
	pTitlePlayer_->Update();

	//bgAngle_ += 0.003f;

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
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
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
		VGet(0, 50, -500),
		VGet(0, 0, 0));

	SetUseBackCulling(FALSE);
	//描画
	bg_.Draw(cameraPos);

	//タイトル用プレイヤーの描画
	pTitlePlayer_->Draw();

	SetUseBackCulling(TRUE);

	//描画
	DrawRotaGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2, 1.0f, 0.0f, titleHandle_, true);

	//タイトル名
	const char* text = "ボタンを押してスタート";
	int textLength = (int)strlen(text);

	int totalWidth = GetDrawStringWidthToHandle(text, textLength, Game::kTitleFontHandle);
	int currentX = (Game::kScreenWidth - totalWidth) / 2;
	int baseY = Game::kScreenHeight / 2 + 100;

	int charOrder = 0;

	for (int i = 0; i < textLength; )
	{
		// --- 1文字のバイト数を判定する ---
		int byteCount = 1;

		//全角文字の対応
		// Shift-JISの場合なら,1バイト目が 0x81-0x9F か 0xE0-0xFC なら全角(2バイト)らしい
		unsigned char c = (unsigned char)text[i];
		if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) {
			byteCount = 2;
		}

		//1文字分だけを取り出す
		char oneChar[3] = { 0 };
		for (int b = 0; b < byteCount; b++) {
			oneChar[b] = text[i + b];
		}

		//文字の揺れの計算
		float waveY = sinf(blinkTimer_ * 0.1f + charOrder * 0.5f) * 10.0f;
		int drawY = static_cast<int>(baseY + waveY);

		//文字の点滅
		int alpha = static_cast<int>(kBlinkBaseAlpha + kBlinkAlphaRange * sinf(blinkTimer_ * kBlinkSpeed));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		//描画
		DrawStringToHandle(currentX + 4, drawY + 4, oneChar, GetColor(0, 0, 0), Game::kTitleFontHandle);
		DrawStringToHandle(currentX, drawY, oneChar, GetColor(255, 255, 255), Game::kTitleFontHandle);

		//次の文字へ進む
		currentX += GetDrawStringWidthToHandle(oneChar, byteCount, Game::kTitleFontHandle);
		i += byteCount;
		charOrder++;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}