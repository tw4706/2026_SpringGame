#pragma once
#include "GameObject.h"

class Input;
class Player :public GameObject
{
public:
	Player();
	virtual~Player();

	void Init()override;
	void Update()override{};
	void Update(Input&input);
	void Draw()override;

	void Move(Input& input);
	void Attack(Input& input);

private:
	int modelHandle_;
};

