#include "PopUI.h"
#include"Game.h"

namespace
{
    //表示する時間
    constexpr float kDispTime = 1.0f;

    //スコアがプレイヤーの頭上に上昇する速さ
    constexpr float kUpSpeed = 1.5f;
}

PopUI::PopUI(const Vector3& pos, int value,ScorePopType type):
	pos_(pos),
	value_(value),
	timer_(0.0f),
	isDead_(false),
    type_(type)
{
}

void PopUI::Update(float dt)
{
    timer_ += dt;

    //上に移動
    pos_.y_ += kUpSpeed;

    if (timer_ > kDispTime)
    {
        isDead_ = true;
    }
}
void PopUI::Draw()
{
    //3Dから2D変換
	//ConvWorldPosToScreenPosは、
    //ワールド座標をスクリーン座標に変換する関数
    VECTOR screen = ConvWorldPosToScreenPos(pos_.ToDxlibVector());

    //スコアのタイプによって表示する内容を変更する
    if (type_ == ScorePopType::Score)
    {
        DrawFormatStringToHandle((int)screen.x,(int)screen.y,
            GetColor(255, 255, 0),Game::kFontHandle,"+%d",value_);
    }
    else if (type_ == ScorePopType::Time)
    {
        DrawFormatStringToHandle(
            (int)screen.x,(int)screen.y,
            GetColor(0, 255, 0),Game::kFontHandle,"+%.1fs",value_ / 1.0f);
    }
}
