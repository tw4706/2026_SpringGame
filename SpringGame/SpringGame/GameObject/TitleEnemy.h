#pragma once
#include "GameObject.h"
#include"Model.h"
#include"../System/Animation.h"

class TitleEnemy :public GameObject
{
public:
	TitleEnemy();
	~TitleEnemy();

	void Init()override;
	void Update()override;
	void Draw()override;

	/// <summary>
	/// “G‚ÌÄ¶¬
	/// </summary>
	void ReSpawn();

	/// <summary>
	/// “G‚Ì€–S
	/// </summary>
	void Death();

private:
	float time_;
	float speed_;
	float angle_;

	Model model_;
	Animation animation_;
};

