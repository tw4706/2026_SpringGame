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
	spawnInteval_ = 0.8f;
}

void EnemySpawner::Update(const Vector3& playerPos, float dt)
{
	//距離の計算
	float distance = (playerPos.x_ - pos_.x_) * (playerPos.x_ - pos_.x_) +
		(playerPos.z_ - pos_.z_) * (playerPos.z_ - pos_.z_);

	float radiusSq = radius_ * radius_;

	isActive_ = (distance <= radiusSq);

	//この距離の範囲に入ったらスポナーを起動させる
	//範囲外だったらスポナーは起動しない
	if (isActive_)
	{
		spawnTimer_ += dt;

		//生成時間になったかつ敵の数が最大数より少ない場合生成する
		if (spawnTimer_ >= spawnInteval_ && pEnemies_.size() < kEnemyMax)
		{
			spawnTimer_ = 0.0f;

			auto enemy = std::make_shared<Enemy>();

			//敵の生成位置をスポナーの周りのランダムな位置から生成
			float offsetX = static_cast<float>(GetRand(200) - 100);
			float offsetZ = static_cast<float>(GetRand(200) - 100);

			Vector3 spawnPos = pos_ + Vector3(offsetX, 0.0f, offsetZ);

			//先に生成位置を決めてから初期化する
			enemy->SetSpawnPos(spawnPos);
			enemy->Init();

			enemy->SetPlayer(pPlayer_);
			enemy->SetCamera(pCamera_);
			enemy->SetScene(pScene_);

			pEnemies_.push_back(enemy);
		}
	}

	//敵の更新
	for (auto& enemies : pEnemies_)
	{
		enemies->Update(dt);
	}

	//削除処理
	pEnemies_.erase(std::remove_if(pEnemies_.begin(), pEnemies_.end(),
		[](std::shared_ptr<Enemy>& e) {return e->IsDestroy(); }), pEnemies_.end());
}

void EnemySpawner::Draw()
{
#ifdef _DEBUG

	//スポナーの生成範囲の描画
	DrawSphere3D(pos_.ToDxlibVector(),radius_,16,GetColor(255, 255, 0),GetColor(255, 255, 0),FALSE);
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
