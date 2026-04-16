#pragma once
#include"../Physics/Vector3.h"
#include<memory>
#include<vector>

class Enemy;
class Player;
class Camera;
class EnemySpawner
{
public:
	EnemySpawner();
	~EnemySpawner();

	void Init(const Vector3&pos,float radius);
	void Update(const Vector3& playerPos, float dt);
	void Draw();

	/// <summary>
	/// 敵の取得
	/// </summary>
	/// <returns></returns>
	const std::vector<std::shared_ptr<Enemy>>& GetEnemy()const;

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player);

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

private:
	Vector3 pos_;		//座標
	float radius_;		//半径
	bool isActive_;		//スポナーが発動したかどうか

	float spawnTimer_;
	float spawnInteval_;

	Player* pPlayer_;
	Camera* pCamera_;
	std::vector<std::shared_ptr<Enemy>>pEnemies_;
};

