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

private:
    float time_;
    float angle_;

    Model model_;
    Animation animation_;
};

