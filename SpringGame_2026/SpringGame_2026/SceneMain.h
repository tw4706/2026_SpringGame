#pragma once
#include<memory>

class Enemy;
class Player;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update();
	void Draw();

private:
	void DrawGrid();

private:
	int frameCount_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Enemy>pEnemy_;
};

