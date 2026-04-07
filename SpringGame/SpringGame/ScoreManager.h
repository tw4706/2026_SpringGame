#pragma once
class ScoreManager
{
public:
	static int AddScore();

	static void Update(float dt);

	static void AddScoreBoost();

	static int GetDispScore() { return dispScore_; }

	static int GetScore() { return score_; }

	static float GetBoostRatio();

	static void Reset();

private:
	static int score_;
	static int dispScore_;
	static float scoreBoostTimer_;
};

