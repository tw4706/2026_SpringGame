#include "ScoreManager.h"

namespace
{
	//スコアのカウントの速さ(数字を大きくすれば早くカウントする)
	constexpr int kCountSpeed = 10;

	//下限の加算値
	constexpr int kMinScore = 1;

	constexpr int kScorePoint = 100;

	constexpr float kScoreBoostTime = 5.0f;
}

int ScoreManager::score_ = 0;
int ScoreManager::dispScore_ = 0;
float ScoreManager::scoreBoostTimer_ = 0.0f;

int ScoreManager::AddScore()
{
	int scorePoint = kScorePoint;
	if (scoreBoostTimer_ > 0.0f)
	{
		scorePoint *= 2;
	}
	score_ += scorePoint;
	return scorePoint;
}

void ScoreManager::Update(float dt)
{
	//スコアブーストするタイマーの更新
	if (scoreBoostTimer_ > 0.0f)
	{
		scoreBoostTimer_ -= dt;

		if (scoreBoostTimer_ < 0.0f)
		{
			scoreBoostTimer_ = 0.0f;
		}
	}

	int diff = score_ - dispScore_;

	if (diff > 0)
	{
		dispScore_ += diff / kCountSpeed + kMinScore;

		//表示スコアが実際のスコアを超えないようにする
		if (dispScore_ > score_)
		{
			dispScore_ = score_;
		}
	}
}

void ScoreManager::AddScoreBoostTime()
{
	scoreBoostTimer_ = kScoreBoostTime;
}

float ScoreManager::GetBoostGauge()
{
	if (scoreBoostTimer_ <= 0.0f)
	{
		return 0.0f;
	}

	float ratio = scoreBoostTimer_ / kScoreBoostTime;

	if (ratio > 1.0f) ratio = 1.0f;
	if (ratio < 0.0f) ratio = 0.0f;

	return ratio;
}

void ScoreManager::Reset()
{
	score_ = 0;
	dispScore_ = 0;
	scoreBoostTimer_ = 0.0f;
}
