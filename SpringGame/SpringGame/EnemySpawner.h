#pragma once
#include"../Physics/Vector3.h"
#include<memory>
#include<vector>

class Enemy;
class EnemySpawner
{
public:
	EnemySpawner();
	~EnemySpawner();

	void Init(const Vector3&pos,float radius);
	void Update(const Vector3&playerPos);

private:
	Vector3 pos_;		//座標
	float radius_;		//半径
	bool isActive_;		//スポナーが発動したかどうか

	float spawnTimer_;
	float spawnInteval_;

	std::vector<std::shared_ptr<Enemy>>pEnemies_;
};

