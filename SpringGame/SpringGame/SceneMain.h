#pragma once
#include<memory>
#include"../Physics/CollisionManager.h"

class Enemy;
class Player;
class Input;
class Camera;
class ScorePop;
class SceneMain
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update(Input& input);
	void Draw();

	void AddScorePop(const Vector3& pos, int value);

private:
	void DrawGrid();

	void DrawSkyQuad(VECTOR a, VECTOR b, VECTOR c, VECTOR d, int tex);

	void DrawSkybox();

private:
	int frameCount_;
	int skyTexture_[6];
	int shadowMap_;

	std::vector<std::shared_ptr<Enemy>> enemies_;
	std::vector<ScorePop>pScorePops_;
	std::shared_ptr<Player>pPlayer_;
	std::shared_ptr<Camera>pCamera_;
	CollisionManager collisionManager_;
};

