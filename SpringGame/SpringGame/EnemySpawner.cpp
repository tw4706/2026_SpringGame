#include "EnemySpawner.h"

EnemySpawner::EnemySpawner():
	pos_{0.0f,0.0f,0.0f},
	radius_(0.0f),
	isActive_(false),
	spawnTimer_(0.0f),
	spawnInteval_(0.0f)
{

}

EnemySpawner::~EnemySpawner()
{

}

void EnemySpawner::Init(const Vector3& pos, float radius)
{
}

void EnemySpawner::Update(const Vector3& playerPos)
{
}
