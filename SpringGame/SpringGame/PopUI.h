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
    PopUI(const Vector3& pos, int value, ScorePopType type);

    void Update(float dt);
    void Draw();

    bool IsDead() const { return isDead_; }

private:
    Vector3 pos_;
    int value_;

    float timer_;
    bool isDead_;

    ScorePopType type_;
};

