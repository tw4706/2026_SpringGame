#pragma once
#include <DxLib.h>

class UIManager
{
public:
	UIManager();
	~UIManager();

	void Init();

	void Draw(int hp,bool isHpAnimating,int damageIndex,int hpAnimFrame,float boostGauge,float time,int score,bool isGameStarted,int gameStartTimer,float timeScale,float bonus,float bonusTimer);

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
	/// スコアブーストのゲージの描画
	/// </summary>
	/// <param name="gauge">ゲージ</param>
	void DrawBoostGauge(float gauge);

	/// <summary>
	/// スコアの描画
	/// </summary>
	/// <param name="score">スコア</param>
	void DrawScore(int score);

	/// <summary>
	/// ボーナスタイムの描画
	/// </summary>
	/// <param name="bonus">ボーナス</param>
	/// <param name="timer">タイマー</param>
	void DrawBonus(float bonus, float timer);

	/// <summary>
	/// ゲーム開始の合図の描画
	/// </summary>
	/// <param name="isGameStarted">ゲームが始まっているならtrue,始まっていないならfalse</param>
	/// <param name="timer">タイマー</param>
	void DrawGameStart(bool isGameStarted, int timer);

	/// <summary>
	/// タイムアップの描画
	/// </summary>
	/// <param name="timer">タイマー</param>
	void DrawTimeUp(float timer);

	/// <summary>
	/// 回避時のスクリーンの描画
	/// </summary>
	/// <param name="timeScale">スローの倍率</param>
	void DrawDodgeScreenColor(float timeScale);

	/// <summary>
	/// 文字列を画面中央に描画する関数
	/// </summary>
	/// <param name="text">描画する文字列</param>
	/// <param name="y">y座標</param>
	/// <param name="color">カラー</param>
	void DrawCenterText(const char* text, int y, int color);
private:
	int handle_;
};

