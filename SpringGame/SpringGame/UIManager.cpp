#include "UIManager.h"
#include"Game.h"
#include <DxLib.h>
#include <string.h>

void UIManager::DrawCenterText(const char* text, int y, int color)
{
	int width = GetDrawStringWidth(text, strlen(text));
	int x = (1280 - width) / 2;

	DrawStringToHandle(x, y, text, color, Game::kFontUIHandle);
}

void UIManager::Draw(int time, int score, float bonus, float bonusTimer)
{
	char buf[64];

	//タイム表示
	sprintf_s(buf, "TIME : %d", time);
	DrawCenterText(buf, 40, GetColor(255, 255, 255));

	//スコア表示
	sprintf_s(buf, "SCORE : %d", score);
	DrawCenterText(buf, 90, GetColor(255, 80, 80));

	//ジャスト回避成功時のボーナス時間の表示
	if (bonusTimer > 0.0f)
	{
		sprintf_s(buf, "+%.1fs", bonus);
		DrawStringToHandle(750,40,buf,
			GetColor(0, 255, 0),Game::kFontUIHandle);
	}
}

