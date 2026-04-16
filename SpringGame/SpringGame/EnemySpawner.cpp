#include "EnemySpawner.h"
#include"../Actor/Enemy.h"

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
	pos_ = pos;
	radius_ = radius;

	isActive_ = false;

	spawnTimer_ = 0.0f;
	spawnInteval_ = 2.0f;
}

void EnemySpawner::Update(const Vector3& playerPos)
{
	//距離
	float distance = (playerPos.x_ - pos_.x_) * (playerPos.x_ - pos_.x_) +
		(playerPos.z_ - pos_.z_) * (playerPos.z_ - pos_.z_);

	float radiusSq = radius_ * radius_;

	//この距離の範囲に入ったらスポナーを起動させる
	if (distance <= radiusSq)
	{
		isActive_ = true;
	}

	//起動していないときは何もしない
	if (!isActive_)return;

	spawnTimer_ += 1.0f / 60.0f;

	if (spawnTimer_ >= spawnInteval_)
	{
		spawnTimer_ = 2.0f;

		auto enemy = std::make_shared<Enemy>();
		enemy->Init();

		pEnemies_.push_back(enemy);
	}

	// 敵更新
	for (auto& e : pEnemies_)
	{
		e->Update();
	}

	//削除処理
	pEnemies_.erase(std::remove_if(pEnemies_.begin(), pEnemies_.end(),
		[](std::shared_ptr<Enemy>& e){return e->IsDestroy();}),pEnemies_.end());
}

void EnemySpawner::Draw()
{
#ifdef _DEBUG
	int x = static_cast<int>(pos_.x_);
	int y = static_cast<int>(pos_.y_);

	int r = static_cast<int>(radius_);

	DrawCircle(x, y, r, GetColor(255, 255, 0), false);

	if (isActive_)
	{
		DrawCircle(x, y, r, GetColor(255, 255, 0), false);
	}
#endif
}

const std::vector<std::shared_ptr<Enemy>>& EnemySpawner::GetEnemy() const
{
	return pEnemies_;
}

void EnemySpawner::SetPlayer(Player* player)
{
	pPlayer_ = player;
}
void EnemySpawner::SetCamera(Camera* camera)
{
	pCamera_ = camera;
}
void EnemySpawner::SetScene(GameScene* scene)
{
	pScene_ = scene;
}
