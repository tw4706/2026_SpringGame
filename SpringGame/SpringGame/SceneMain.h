#pragma once
#include<memory>
#include"../Physics/CollisionManager.h"

class Enemy;
class Player;
class Input;
class Camera;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();

private:
	void DrawGrid();

private:
	int frameCount_;
	std::shared_ptr<Enemy>pEnemy_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	CollisionManager collisionManager_;
};

