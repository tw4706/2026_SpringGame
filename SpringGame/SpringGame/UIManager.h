#pragma once
#include <DxLib.h>

class UIManager
{
public:
	void Draw(int time, int score, float bonus, float bonusTimer);

private:
	void DrawCenterText(const char* text, int y, int color);
};

