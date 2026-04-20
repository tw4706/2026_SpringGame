#pragma once
#include <DxLib.h>

class GameSceneUI
{
public:
	GameSceneUI();
	~GameSceneUI();

	void Init();

	void Draw(int hp,bool isHpAnimating,int damageIndex,int hpAnimFrame,float time,bool isGameStarted, float gameStartTimer,float timeScale, int currentWave);

private:

	/// <summary>
	/// 文字の描画
	/// </summary>
	/// <param name="time"></param>
	void DrawTime(float time);

	/// <summary>
	/// HPUIの描画
	/// </summary>
	void DrawHPUI(int hp,bool isAnimating,int damageIndex,int animFrame);

	/// <summary>
	/// ゲーム開始の合図の描画
	/// </summary>
	/// <param name="isGameStarted">ゲームが始まっているならtrue,始まっていないならfalse</param>
	/// <param name="timer">タイマー</param>
	void DrawGameStart(bool isGameStarted, float gameStartTimer);

	/// <summary>
	/// 回避時のスクリーンの描画
	/// </summary>
	/// <param name="timeScale">スローの倍率</param>
	void DrawDodgeScreenColor(float timeScale);

	/// <summary>
	/// 何ウェーブかの描画
	/// </summary>
	void DrawWave(int currentWave);

	/// <summary>
	/// 文字列を画面中央に描画する関数
	/// </summary>
	/// <param name="text">描画する文字列</param>
	/// <param name="y">y座標</param>
	/// <param name="color">カラー</param>
	void DrawCenterText(const char* text, int y, int color, float scale);
private:
	int handle_;
};

