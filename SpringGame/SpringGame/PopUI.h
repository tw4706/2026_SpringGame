#pragma once
#include"../Physics/Vector3.h"

enum class ScorePopType
{
    Score,
    Time
};

class PopUI
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="pos">座標</param>
    /// <param name="value">大きさ</param>
    /// <param name="type">スコアポップタイプ</param>
    PopUI(const Vector3& pos, int value, ScorePopType type);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="dt">経過時間</param>
    void Update(float dt);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// 死亡しているかどうか
    /// </summary>
    /// <returns>UIが消えていたらtrue,それ以外はfalse</returns>
    bool IsDead() const { return isDead_; }

private:
    Vector3 pos_;
    int value_;

    float timer_;
    bool isDead_;

    ScorePopType type_;
};

