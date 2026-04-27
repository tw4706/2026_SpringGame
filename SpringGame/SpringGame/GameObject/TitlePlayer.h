#pragma once
#include "GameObject.h"
#include"Model.h"
#include "../System/Animation.h"

class TitlePlayer :public GameObject
{
public:
    TitlePlayer();
    ~TitlePlayer();

    void Init() override;
    void Update()override;
    void Draw() override;

    /// <summary>
    /// çUåÇ
    /// </summary>
    void Attack();

    bool IsAttacking()const { return isAttacking_; }

private:
    float time_;
    float angle_;
    bool isAttacking_;

    Model model_;
    Animation animation_;
};

