#include "EnemySpawner.h"
#include"../Actor/Enemy.h"

namespace
{
	constexpr int kEnemyMax = 3;
}

EnemySpawner::EnemySpawner() :
	pos_{ 0.0f,0.0f,0.0f },
	radius_(0.0f),
	isActive_(false),
	spawnTimer_(0.0f),
	spawnInteval_(0.0f),
	pPlayer_(nullptr),
	pCamera_(nullptr),
	pScene_(nullptr)
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

	//生成時間になったかつ敵の数が最大数より少ない場合生成する
	if (spawnTimer_ >= spawnInteval_ && pEnemies_.size() < kEnemyMax)
	{
		spawnTimer_ = 0.0f;

		auto enemy = std::make_shared<Enemy>();
		enemy->Init();

		enemy->SetPlayer(pPlayer_);
		enemy->SetCamera(pCamera_);
		enemy->SetScene(pScene_);

		pEnemies_.push_back(enemy);
	}

	// 敵更新
	for (auto& e : pEnemies_)
	{
		e->Update();
	}

	//削除処理
	pEnemies_.erase(std::remove_if(pEnemies_.begin(), pEnemies_.end(),
		[](std::shared_ptr<Enemy>& e) {return e->IsDestroy(); }), pEnemies_.end());
}

void EnemySpawner::Draw()
{
#ifdef _DEBUG
	Vector3 screenPos;
	ConvWorldPosToScreenPos(screenPos.ToDxlibVector());

	int x = (int)screenPos.x_;
	int y = (int)screenPos.y_;

	DrawSphere3D(screenPos.ToDxlibVector(), (int)radius_, 16, 0xffff00, 0xffff00, false);
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
