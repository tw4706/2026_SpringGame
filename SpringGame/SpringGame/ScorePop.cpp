#include "ScorePop.h"

namespace
{
    //表示する時間
    constexpr float kDispTime = 1.0f;

    //スコアがプレイヤーの頭上に上昇する速さ
    constexpr float kUpSpeed = 1.5f;
}

ScorePop::ScorePop(const Vector3& pos, int value):
	pos_(pos),
	value_(value),
	timer_(0.0f),
	isDead_(false)
{
}

void ScorePop::Update()
{
    timer_ += 1.0f / 60.0f;

    //上に移動
    pos_.y_ += kUpSpeed;

    if (timer_ > kDispTime)
    {
        isDead_ = true;
    }
}
void ScorePop::Draw()
{
    //3Dから2D変換
	//ConvWorldPosToScreenPosは、
    //ワールド座標をスクリーン座標に変換する関数
    VECTOR screen = ConvWorldPosToScreenPos(pos_.ToDxlibVector());

    DrawFormatString((int)screen.x,(int)screen.y,
        GetColor(255, 255, 0),"+%d", value_);
}
