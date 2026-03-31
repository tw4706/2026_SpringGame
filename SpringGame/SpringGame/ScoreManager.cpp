#include "ScoreManager.h"

namespace
{
	//スコアのカウントの速さ(数字を大きくすれば早くカウントする)
	constexpr int kCountSpeed = 10;

	//下限の加算値
	constexpr int kMinScore = 1;
}

int ScoreManager::score_ = 0;
int ScoreManager::dispScore_ = 0;

void ScoreManager::AddScore(int score)
{
	score_ += score;
}

void ScoreManager::Update()
{
	int diff = score_ - dispScore_;

	if (diff > 0)
	{
		dispScore_ += diff / kCountSpeed + kMinScore;
	}
}

int ScoreManager::GetScore()
{
	return dispScore_;
}

void ScoreManager::Reset()
{
	score_ = 0;
	dispScore_ = 0;
}
