#include "UIManager.h"
#include"../Game.h"
#include"../Manager/ScoreManager.h"
#include <DxLib.h>
#include <string>
#include <algorithm>

namespace
{
	//HPUIのハートの最大数
	constexpr int kMaxHP = 3;

	//HPUIのアニメーションの感覚
	constexpr float kHpAnimInterval = 0.05f;
	//HPUIを最後の画像で止めるためにそのアニメーションの最大数
	constexpr int kHpAnimMaxFrame = 11;

	//HPUIの描画するための定数(縦・横,拡大率)
	constexpr int kFrameW = 32;
	constexpr int kFrameH = 32;
	constexpr float kHpScale = 2.0f;

	//ゲーム開始の合図を出すフレーム数
	constexpr int kReadyFrame = 60;
	constexpr int kStartFrame = 30;
}

UIManager::UIManager():
	handle_(-1)
{
}

UIManager::~UIManager()
{
	DeleteGraph(handle_);
}

void UIManager::Init()
{
	handle_ = LoadGraph("data/HP.png");
}

void UIManager::Draw(int hp, bool isHpAnimating, int damageIndex, int hpAnimFrame,
	float boostGauge, float time, int score, bool isGameStarted,
	int gameStartTimer, float timeScale, float bonus, float bonusTimer)
{
	DrawTime(time);
	DrawScore(score);
	DrawBonus(bonus, bonusTimer);
	DrawHPUI(hp, isHpAnimating, damageIndex, hpAnimFrame);
	DrawBoostGauge(boostGauge);
	DrawGameStart(isGameStarted, gameStartTimer);
	DrawDodgeScreenColor(timeScale);
}

void UIManager::DrawHPUI(int hp, bool isAnimating, int damageIndex, int animFrame)
{
	//HPUI描画
	int x = 50;
	int y = 50;

	//拡大後サイズ
	int drawW = static_cast<int>(kFrameW * kHpScale);
	int drawH = static_cast<int>(kFrameH * kHpScale);

	//最大3このハートを描画する

	for (int i = 0; i < kMaxHP; i++)
	{
		int drawX = x + i * (drawW + 10);

		//中心座標を指定する
		int centerX = drawX + (int)(kFrameW * kHpScale);
		int centerY = y + (int)(kFrameH * kHpScale);

		int srcX = 0;

		if (isAnimating && i == damageIndex)srcX = animFrame * kFrameW;
		else if (i < hp)srcX = 0;
		else srcX = (kHpAnimMaxFrame - 1) * kFrameW;

		DrawRectRotaGraph(centerX, centerY,
			srcX, 192,
			kFrameW, kFrameH,
			kHpScale, 0.0,
			handle_, TRUE, FALSE);
	}
}

void UIManager::DrawBoostGauge(float gauge)
{
	if (gauge <= 0.0f) return;

	int x = 50, y = 110;
	int w = 220, h = 18;

	int currentW = (int)(w * gauge);

	DrawBox(x, y, x + w, y + h, GetColor(255, 255, 255), FALSE);
	DrawBox(x + 2, y + 2, x + currentW - 2, y + h - 2, GetColor(255, 220, 0), TRUE);
}

void UIManager::DrawScore(int score)
{
	char buf[64];
	sprintf_s(buf, "SCORE : %d", score);

	//スコアの表示
	DrawCenterText(buf, 90, GetColor(255, 80, 80));
}

void UIManager::DrawBonus(float bonus, float timer)
{
	if (timer <= 0.0f) return;

	char buf[64];
	sprintf_s(buf, "+%.1fs", bonus);

	//ボーナスの描画
	DrawStringToHandle(750, 40, buf, GetColor(0, 255, 0), Game::kFontUIHandle);
}

void UIManager::DrawGameStart(bool isGameStarted, int timer)
{
	//開始の合図の描画
	if (!isGameStarted)
	{
		//スタートタイマーが60フレームより小さい場合はReadyそれ以降はGo!
		if (timer < kReadyFrame)
		{
			DrawCenterText("READY", Game::kScreenHeight / 2, GetColor(255, 255, 0));
		}
		else
		{
			DrawCenterText("Go!", Game::kScreenHeight / 2, GetColor(0, 255, 255));
		}
	}
}

void UIManager::DrawDodgeScreenColor(float timeScale)
{
	if (timeScale >= 1.0f) return;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawBox(0, 0, 1280, 720, GetColor(180, 100, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void UIManager::DrawCenterText(const char* text, int y, int color)
{
	int width = GetDrawStringWidthToHandle(text, strlen(text), Game::kFontUIHandle);
	int x = (Game::kScreenWidth - width) / 2;

	DrawStringToHandle(x, y, text, color, Game::kFontUIHandle);
}

void UIManager::DrawTime(float time)
{
	char buf[64];
	sprintf_s(buf, "%.2f", time);
	DrawCenterText("TIME:", 40, GetColor(255, 255, 255));

	//Timeの表示
	DrawCenterText(buf, 40, GetColor(255, 255, 255));
}

