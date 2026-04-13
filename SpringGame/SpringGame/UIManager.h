#pragma once
#include <DxLib.h>

class UIManager
{
public:
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="time">時間</param>
	/// <param name="score">スコア</param>
	/// <param name="bonus">ボーナス</param>
	/// <param name="bonusTimer">ボーナス時間</param>
	void Draw(int time, int score, float bonus, float bonusTimer);

private:

	/// <summary>
	/// 文字列を画面中央に描画する関数
	/// </summary>
	/// <param name="text">描画する文字列</param>
	/// <param name="y">y座標</param>
	/// <param name="color">カラー</param>
	void DrawCenterText(const char* text, int y, int color);
};

