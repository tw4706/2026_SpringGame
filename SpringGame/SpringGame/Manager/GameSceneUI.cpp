#include "GameSceneUI.h"
#include"../Game.h"
#include"../System/Application.h"
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
	constexpr float kReadyFrame = 60.0f;
	constexpr float kStartFrame = 30.0f;
}

GameSceneUI::GameSceneUI() :
	handle_(-1)
{
}

GameSceneUI::~GameSceneUI()
{
	DeleteGraph(handle_);
}

void GameSceneUI::Init()
{
	handle_ = LoadGraph("data/HP.png");
}

void GameSceneUI::Draw(int hp, bool isHpAnimating, int damageIndex, int hpAnimFrame,
	float time, bool isGameStarted,
	float gameStartTimer, float timeScale, float bonus, float bonusTimer, int currentWave)
{
	DrawTime(time);
	DrawHPUI(hp, isHpAnimating, damageIndex, hpAnimFrame);
	DrawGameStart(isGameStarted, gameStartTimer);
	DrawDodgeScreenColor(timeScale);
	DrawWave(currentWave);
}

void GameSceneUI::DrawHPUI(int hp, bool isAnimating, int damageIndex, int animFrame)
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
		int centerX = drawX + (int)(kFrameW * kHpScale * 0.5f);
		int centerY = y + (int)(kFrameH * kHpScale * 0.5f);

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

void GameSceneUI::DrawGameStart(bool isGameStarted, float gameStartTimer)
{
	//開始の合図の描画
	if (!isGameStarted)
	{
		//スタートタイマーが60フレームより小さい場合はReadyそれ以降はGo!
		if (gameStartTimer <= kReadyFrame)
		{
			DrawCenterText("READY", Game::kScreenHeight / 2, GetColor(255, 255, 0));
			Application::GetInstance().GetSoundManager().PlaySe(SE::CountDown);
		}
		else
		{
			DrawCenterText("Go!", Game::kScreenHeight / 2, GetColor(0, 255, 255));
			Application::GetInstance().GetSoundManager().PlaySe(SE::Start);
		}
	}
}

void GameSceneUI::DrawDodgeScreenColor(float timeScale)
{
	if (timeScale >= 1.0f) return;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	DrawBox(0, 0, 1280, 720, GetColor(180, 100, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameSceneUI::DrawWave(int currentWave)
{
	const int maxWave = 6;

	char buf[64];
	sprintf_s(buf, "WAVE %d / %d", currentWave, maxWave);

	int y = 150;

	//中央表示
	int width = GetDrawStringWidthToHandle(buf, strlen(buf), Game::kFontUIHandle);
	int x = 50;

	//影付きで描画
	DrawStringToHandle(x + 4, y + 4, buf, GetColor(0, 0, 0), Game::kFontUIHandle);
	DrawStringToHandle(x, y, buf, GetColor(255, 255, 255), Game::kFontUIHandle);
}

void GameSceneUI::DrawCenterText(const char* text, int y, int color)
{
	int width = GetDrawStringWidthToHandle(text, static_cast<int>(strlen(text)), Game::kFontUIHandle);
	int x = (Game::kScreenWidth - width) / 2;

	DrawStringToHandle(x, y, text, color, Game::kFontUIHandle);
}

void GameSceneUI::DrawTime(float time)
{
	char buf[64];
	const char* timeText = "TIME :";
	sprintf_s(buf, "%.2f", time);

	//基準位置
	int baseX = Game::kScreenWidth / 2;
	int y = 40;

	//"TIME :" の幅
	int timeWidth = GetDrawStringWidthToHandle(timeText, static_cast<int>(strlen(timeText)), Game::kFontUIHandle);

	//数字の幅の最大を決めることで数字がずれても最低この値までは保証される
	int numberWidth = GetDrawStringWidthToHandle("00.00", static_cast<int>(strlen("00.00")), Game::kFontUIHandle);

	//全体を中央にする
	int startX = baseX - (timeWidth + 10 + numberWidth) / 2;

	int x = startX + timeWidth + 10;

	//タイム
	DrawStringToHandle(startX + 4, y + 4, timeText, GetColor(0, 0, 0), Game::kFontUIHandle);
	DrawStringToHandle(startX, y, timeText, GetColor(255, 255, 255), Game::kFontUIHandle);

	//数字
	DrawStringToHandle(x + 4, y + 4, buf, GetColor(0, 0, 0), Game::kFontUIHandle);
	DrawStringToHandle(x, y, buf, GetColor(255, 255, 255), Game::kFontUIHandle);
}

