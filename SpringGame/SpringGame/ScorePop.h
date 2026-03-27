#pragma once
#include"../Physics/Vector3.h"

class ScorePop
{
public:
    ScorePop(const Vector3& pos, int value);

    void Update();
    void Draw();

    bool IsDead() const { return isDead_; }

private:
    Vector3 pos_;
    int value_;

    float timer_;
    bool isDead_;
};

