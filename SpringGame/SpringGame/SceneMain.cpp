#include "SceneMain.h"
#include"../Actor/Player.h"
#include"../Actor/Enemy.h"
#include"../Input.h"
#include"../Physics/Camera.h"
#include <Dxlib.h>

SceneMain::SceneMain() :
	frameCount_(0)
{
	pEnemy_ = std::make_shared<Enemy>();
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
	SetCameraNearFar(1.0f, 1500.0f);

	//各クラスの初期化処理
	pEnemy_->Init();
	pPlayer_->Init();
	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();
}

void SceneMain::Update(Input&input)
{
	frameCount_++;

	//各クラスの更新処理
	pEnemy_->Update();
	pPlayer_->Update(input);
	pCamera_->Update();

	//当たり判定の処理
	collisionManager_.Clear();

	//当たり判定の登録
	collisionManager_.AddCollider(pPlayer_->GetCollider());
	collisionManager_.AddCollider(pPlayer_->GetAttackCollider());
	collisionManager_.AddCollider(pEnemy_->GetCollider());

	//衝突判定
	collisionManager_.CheckAllCollision();
}

void SceneMain::Draw()
{
	DrawGrid();

	//各クラスの描画処理
	pEnemy_->Draw();
	pPlayer_->Draw();

	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", frameCount_);
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

	const int GRID_NUM = 6;      // 片側のマス数
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
