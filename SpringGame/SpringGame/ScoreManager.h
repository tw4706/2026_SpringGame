#pragma once
class ScoreManager
{
public:
	static void AddScore(int score);

	static void Update();

	static int GetScore();

	static void Reset();

private:
	static int score_;
	static int dispScore_;
};

