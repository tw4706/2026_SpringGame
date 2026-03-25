#pragma once

enum class AnimationState
{
    Idle,
    Run,
    Attack,
    Death
};

class Animation
{
public:
	Animation();
	~Animation();

	void Init(int modelHandle);
    void Update(float deltaTime);

	//モデルのアニメーションの再生
	void Play(int animIndex, float speed, bool isLoop);

    void ChangeState(AnimationState state);


private:
	int modelHandle_;
    //現在アニメ
    int currentAttach_;
    int currentAnim_;
    float currentTime_;

    float speed_;
    bool isLoop_;

    float totalTime_;

    AnimationState state_;
};

