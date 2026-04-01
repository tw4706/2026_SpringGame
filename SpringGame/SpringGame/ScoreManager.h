#pragma once
class ScoreManager
{
public:
	static void AddScore();

	static void Update();

	static int GetDispScore() { return dispScore_; }

	static int GetScore() { return score_; }

	static void Reset();

private:
	static int score_;
	static int dispScore_;
};

