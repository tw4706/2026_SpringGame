#include "SceneMain.h"
#include"../Actor/Player.h"
#include"../Actor/Enemy.h"
#include"../Input.h"
#include"../Physics/Camera.h"
#include"../ScoreManager.h"
#include <Dxlib.h>

namespace
{
	constexpr float kEnemyMax = 3;
}

SceneMain::SceneMain() :
	frameCount_(0)
{
	pPlayer_ = std::make_shared<Player>();
	pCamera_ = std::make_shared<Camera>();
}

SceneMain::~SceneMain()
{
	
}

void SceneMain::Init()
{
	// カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(false);

	// Zバッファの設定
	SetUseZBuffer3D(true);		// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(10.0f, 2000.0f);

	skyTexture_[0] = LoadGraph("data/backGround_rt.png");
	skyTexture_[1] = LoadGraph("data/backGround_lf.png");
	skyTexture_[2] = LoadGraph("data/backGround_up.png");
	skyTexture_[3] = LoadGraph("data/backGround_dn.png");
	skyTexture_[4] = LoadGraph("data/backGround_ft.png");
	skyTexture_[5] = LoadGraph("data/backGround_bk.png");

	//各クラスの初期化処理
	for (int i = 0; i < kEnemyMax; i++)
	{
		auto enemy = std::make_shared<Enemy>();
		enemy->Init();
		enemy->SetPlayer(pPlayer_.get());

		float range = 1000.0f;

		float x = ((float)rand() / RAND_MAX) * range * 2 - range;
		float z = ((float)rand() / RAND_MAX) * range * 2 - range;

		enemy->SetPos({ x, 0.0f, z });

		enemies_.push_back(enemy);
	}
	
	pPlayer_->Init();
	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();
}

void SceneMain::Update(Input&input)
{
	frameCount_++;

	//各クラスの更新処理
	ScoreManager::Update();

	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}
	pPlayer_->Update(input);
	pCamera_->Update();

	//当たり判定の処理
	collisionManager_.Clear();

	//当たり判定の登録
	collisionManager_.AddCollider(pPlayer_->GetCollider());
	collisionManager_.AddCollider(pPlayer_->GetAttackCollider());
	for (auto& enemy : enemies_)
	{
		collisionManager_.AddCollider(enemy->GetCollider());
	}

	//衝突判定
	collisionManager_.CheckAllCollision();

	//敵の削除処理
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::shared_ptr<Enemy>& e)
			{
				return e->IsDestroy();
			}),
		enemies_.end());

	//常時何体かわいているようにする
	while (enemies_.size() < kEnemyMax)
	{
		auto enemy = std::make_shared<Enemy>();

		float range = 1000.0f;

		float x = ((float)rand() / RAND_MAX) * range * 2 - range;
		float z = ((float)rand() / RAND_MAX) * range * 2 - range;

		enemy->SetPos({ x, 0.0f, z });
		enemy->SetPlayer(pPlayer_.get());
		enemy->SetCamera(pCamera_.get());
		enemy->Init();
		enemies_.push_back(enemy);
	}
}

void SceneMain::Draw()
{
	DrawSkybox();

	DrawGrid();

	//各クラスの描画処理
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
	pPlayer_->Draw();
#ifdef DEBUG_
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", frameCount_);
#endif

	//点数表示
	DrawFormatString(0, 32, GetColor(255, 0, 0),"Score : %d", ScoreManager::GetScore());
}

void SceneMain::DrawGrid()
{
	//// 直線の始点と終点
	//VECTOR startPos;
	//VECTOR endPos;
	//for (int z = -300; z <= 300; z += 100)
	//{
	//	startPos = VGet(-300.0f, 0.0f, static_cast<float>(z));
	//	endPos = VGet(300.0f, 0.0f, static_cast<float>(z));
	//	DrawLine3D(startPos, endPos, 0xff0000);
	//}
	//for (int x = -300; x <= 300; x += 100)
	//{
	//	startPos = VGet(static_cast<float>(x), 0.0f, -300.0f);
	//	endPos = VGet(static_cast<float>(x), 0.0f, 300.0f);
	//	DrawLine3D(startPos, endPos, 0x0000ff);
	//}

	const int GRID_NUM = 10;      // 片側のマス数
	const float TILE_SIZE = 100.0f;

	for (int z = -GRID_NUM; z < GRID_NUM; z++)
	{
		for (int x = -GRID_NUM; x < GRID_NUM; x++)
		{
			VECTOR v1 = VGet(x * TILE_SIZE, 0.0f, z * TILE_SIZE);
			VECTOR v2 = VGet((x + 1) * TILE_SIZE, 0.0f, z * TILE_SIZE);
			VECTOR v3 = VGet((x + 1) * TILE_SIZE, 0.0f, (z + 1) * TILE_SIZE);
			VECTOR v4 = VGet(x * TILE_SIZE, 0.0f, (z + 1) * TILE_SIZE);

			int color = GetColor(80, 160, 80);

			// 三角形2枚で四角形
			DrawTriangle3D(v1, v2, v3, color, TRUE);
			DrawTriangle3D(v1, v3, v4, color, TRUE);
		}
	}
}

void SceneMain::DrawSkyQuad(VECTOR a, VECTOR b, VECTOR c, VECTOR d, int tex)
{
	VERTEX3D v[4];

	v[0].pos = a; v[0].u = 0; v[0].v = 0;
	v[1].pos = b; v[1].u = 1; v[1].v = 0;
	v[2].pos = c; v[2].u = 1; v[2].v = 1;
	v[3].pos = d; v[3].u = 0; v[3].v = 1;

	for (int i = 0; i < 4; i++)
	{
		v[i].dif.r = 255;
		v[i].dif.g = 255;
		v[i].dif.b = 255;
		v[i].dif.a = 255;

		v[i].spc.r = 0;
		v[i].spc.g = 0;
		v[i].spc.b = 0;
		v[i].spc.a = 0;
	}

	// 三角形1
	DrawPolygon3D(&v[0], 3, tex, TRUE);

	// 三角形2
	VERTEX3D v2[3] = { v[0], v[2], v[3] };
	DrawPolygon3D(v2, 3, tex, TRUE);
}

void SceneMain::DrawSkybox()
{
	//SetUseBackCulling(false);

	float size = 400.0f;

	VECTOR cam = GetCameraPosition();

	// 8頂点
	VECTOR v[8] =
	{
		VGet(cam.x - size, cam.y + size, cam.z - size),
		VGet(cam.x + size, cam.y + size, cam.z - size),
		VGet(cam.x + size, cam.y - size, cam.z - size),
		VGet(cam.x - size, cam.y - size, cam.z - size),

		VGet(cam.x - size, cam.y + size, cam.z + size),
		VGet(cam.x + size, cam.y + size, cam.z + size),
		VGet(cam.x + size, cam.y - size, cam.z + size),
		VGet(cam.x - size, cam.y - size, cam.z + size),
	};

	SetUseLighting(FALSE);
	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetDrawZ(0.0f);
	// 前
	DrawSkyQuad(v[4], v[5], v[6], v[7], skyTexture_[4]);

	// 後
	DrawSkyQuad(v[1], v[0], v[3], v[2], skyTexture_[5]);

	// 左
	DrawSkyQuad(v[4], v[0], v[3], v[7], skyTexture_[1]);

	// 右 ←修正
	DrawSkyQuad(v[1], v[5], v[6], v[2], skyTexture_[0]);

	// 上
	DrawSkyQuad(v[0], v[1], v[5], v[4], skyTexture_[2]);

	// 下
	DrawSkyQuad(v[7], v[6], v[2], v[3], skyTexture_[3]);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetDrawZ(1.0f);
	SetUseLighting(TRUE);

//	printfDx("cam: %f %f %f\n", cam.x, cam.y, cam.z);
}
