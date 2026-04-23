#include "EnemySpawner.h"
#include"../GameObject/Enemy.h"
#include"../System/Application.h"
#include"../Manager/EffectManager.h"
#include"EffekseerForDXLib.h"

namespace
{
	constexpr int kEnemyMax = 5;
}

EnemySpawner::EnemySpawner() :
	areaLockHandle_(-1),
	pos_{ 0.0f,0.0f,0.0f },
	radius_(0.0f),
	isActive_(false),
	isLocked_(false),
	isCleared_(false),
	isSpawned_(false),
	spawnTimer_(0.0f),
	spawnInteval_(0.0f),
	pPlayer_(nullptr),
	pCamera_(nullptr)
{

}

EnemySpawner::~EnemySpawner()
{
	EffectManager::GetInstance().Stop(areaLockHandle_);
}

void EnemySpawner::Init(const Vector3& pos, float radius)
{
	pos_ = pos;
	radius_ = radius;

	isActive_ = false;
	isLocked_ = false;
	isCleared_ = false;
	isSpawned_ = false;

	spawnTimer_ = 0.0f;
	spawnInteval_ = 0.1f;
}

void EnemySpawner::Update(const Vector3& playerPos, float dt)
{
	if (areaLockHandle_ != -1)
	{
		SetPosPlayingEffekseer3DEffect(areaLockHandle_,pos_.x_, pos_.y_, pos_.z_);
	}

	//距離の計算
	float distance = (playerPos.x_ - pos_.x_) * (playerPos.x_ - pos_.x_) +
		(playerPos.z_ - pos_.z_) * (playerPos.z_ - pos_.z_);

	float radiusSq = radius_ * radius_;

	isActive_ = (distance <= radiusSq);

	// プレイヤーが範囲に入ったらエリアの制限を開始(エフェクトも出す)
	if (!isLocked_ && !isCleared_ && distance <= radiusSq)
	{
		isLocked_ = true;

		//エフェクト再生
		areaLockHandle_ = EffectManager::GetInstance().Play("areaLock", pos_);

		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::AreaLock);
	}

	//この距離の範囲に入ったらスポナーを起動させる
	//範囲外だったらスポナーは起動しない
	if (isLocked_ && !isCleared_ && !isSpawned_)
	{
		for (int i = 0; i < kEnemyMax; i++)
		{
			auto enemy = std::make_shared<Enemy>();

			float offsetX = static_cast<float>(GetRand(800) - 400);
			float offsetZ = static_cast<float>(GetRand(800) - 400);

			Vector3 spawnPos = pos_ + Vector3(offsetX, 0.0f, offsetZ);

			enemy->SetSpawnPos(spawnPos);
			enemy->Init();
			enemy->SetPlayer(pPlayer_);
			enemy->SetCamera(pCamera_);

			pEnemies_.push_back(enemy);
		}

		isSpawned_ = true;
	}

	//そのスポナーの周りの敵が全滅しているなら行動範囲の制限解除
	if (isLocked_ && isSpawned_ && pEnemies_.empty())
	{
		isLocked_ = false;
		isCleared_ = true;
	}

	//敵の更新
	for (auto& enemies : pEnemies_)
	{
		enemies->Update(dt);
	}

	//ハンドルの削除処理
	if (isCleared_)
	{
		if (areaLockHandle_ != -1)
		{
			//SE再生
			Application::GetInstance().GetSoundManager().PlaySe(SE::AreaRelease);

			EffectManager::GetInstance().Stop(areaLockHandle_);
			areaLockHandle_ = -1;
		}
	}

	//削除処理
	pEnemies_.erase(std::remove_if(pEnemies_.begin(), pEnemies_.end(),
		[&](std::shared_ptr<Enemy>& e)
		{
			//削除
			if (e->IsDestroy())
			{
				return true;
			}

			//プレイヤーとの距離
			Vector3 enemyPos = e->GetPos();
			float dx = enemyPos.x_ - playerPos.x_;
			float dz = enemyPos.z_ - playerPos.z_;
			float distanceSq = dx * dx + dz * dz;

			//1000以上離れた敵は削除する
			return distanceSq >= 1500.0f * 1500.0f;
		}),
		pEnemies_.end());
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

void EnemySpawner::StopEffect()
{
	if (areaLockHandle_ != -1)
	{
		StopEffekseer3DEffect(areaLockHandle_);
		areaLockHandle_ = -1;
	}
}
