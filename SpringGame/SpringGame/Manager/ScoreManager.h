#pragma once
class ScoreManager
{
public:

	/// <summary>
	/// スコアの加算
	/// </summary>
	/// <returns>スコア</returns>
	static int AddScore();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	static void Update(float dt);

	/// <summary>
	/// ボーナスタイムの加算
	/// </summary>
	static void AddScoreBoostTime();

	/// <summary>
	/// 表示するスコアの取得
	/// </summary>
	/// <returns>表示するスコア</returns>
	static int GetDispScore() { return dispScore_; }

	/// <summary>
	/// スコアの取得
	/// </summary>
	/// <returns>スコア</returns>
	static int GetScore() { return score_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	static float GetBoostGauge();

	/// <summary>
	/// スコアやタイマーのリセット
	/// </summary>
	static void Reset();

private:
	static int score_;
	static int dispScore_;
	static float scoreBoostTimer_;
};

