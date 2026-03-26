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

	void DrawSkyQuad(VECTOR a, VECTOR b, VECTOR c, VECTOR d, int tex);

	void DrawSkybox();

private:
	int frameCount_;
	int skyTexture_[6];
	std::vector<std::shared_ptr<Enemy>> enemies_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	CollisionManager collisionManager_;
};

