#include "SceneMain.h"
#include"../Actor/Player.h"
#include"../Actor/Enemy.h"
#include"../Input.h"
#include"../Physics/Camera.h"
#include"../ScoreManager.h"
#include"../ScorePop.h"
#include"SceneController.h"
#include "ClearScene.h"
#include<algorithm>
#include <Dxlib.h>

namespace
{
	//敵の最大で出現する数
	constexpr float kEnemyMax = 3;

	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//制限時間
	constexpr float kClearFadeTime = 60.0f;
}

SceneMain::SceneMain(SceneController& contorller) :
	Scene(contorller),
	frameCount_(0),
	playTime_(0.0f),
	skyTexture_{},
	update_(&SceneMain::FadeInUpdate),
	draw_(&SceneMain::FadeDraw)
{
	pPlayer_ = std::make_shared<Player>();
	pCamera_ = std::make_shared<Camera>();
}

SceneMain::~SceneMain()
{

}

void SceneMain::Init()
{
	//カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(false);

	//Zバッファの設定
	SetUseZBuffer3D(true);		//Zバッファを使います
	SetWriteZBuffer3D(true);	//描画する物体はZバッファにも距離を書き込む

	//カメラ設定
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(10.0f, 2000.0f);
	SetFontSize(40);

	//スカイボックス用のテクスチャのロード
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
		enemy->SetScene(this);

		//敵をランダムな位置に配置する
		float range = 1000.0f;
		float x = ((float)rand() / RAND_MAX) * range * 2 - range;
		float z = ((float)rand() / RAND_MAX) * range * 2 - range;

		enemy->SetPos({ x, 0.0f, z });

		enemies_.push_back(enemy);
	}

	pPlayer_->Init();
	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();

	frameCount_ = kFadeInterval;
}

void SceneMain::Update(Input& input)
{
	(this->*update_)(input);
}

void SceneMain::Draw()
{
	(this->*draw_)();
}

void SceneMain::AddScorePop(const Vector3& pos, int value)
{
	pScorePops_.emplace_back(pos, value);
}

void SceneMain::DrawCenterTextWithOutline(const char* text, int y, int color, int screenW)
{
	int width = GetDrawStringWidth(text, strlen(text));
	int x = (screenW - width) / 2;

	//太い縁取り
	for (int dy = -3; dy <= 3; dy++)
	{
		for (int dx = -3; dx <= 3; dx++)
		{
			if (dx == 0 && dy == 0) continue;

			DrawString(x + dx, y + dy, text, GetColor(0, 0, 0));
		}
	}

	//本体
	DrawString(x, y, text, color);
}

void SceneMain::FadeInUpdate(Input& input)
{
	mode_ = FadeMode::In;

	pPlayer_->Update(input);
	pCamera_->Update();

	if (frameCount_-- <= 0)
	{
		update_ = &SceneMain::NormalUpdate;
		draw_ = &SceneMain::NormalDraw;
	}
}

void SceneMain::NormalUpdate(Input& input)
{
	frameCount_++;

	//制限時間の更新
	playTime_ += 1.0f / 60.0f;

	//スコアの更新処理
	ScoreManager::Update();

	//各クラスの更新処理
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}

	for (auto& p : pScorePops_)
	{
		p.Update();
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

	//消す
	pScorePops_.erase(
		std::remove_if(pScorePops_.begin(), pScorePops_.end(),
			[](const ScorePop& p) { return p.IsDead(); }),
		pScorePops_.end());

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
		enemy->SetScene(this);
		enemy->Init();
		enemies_.push_back(enemy);
	}

	//60秒経ったらクリアシーンへ
	if (playTime_ >= kClearFadeTime)
	{
		controller_.ChangeScene(std::make_shared<ClearScene>(controller_));
	}
}

void SceneMain::FadeOutUpdate(Input& input)
{
	mode_ = FadeMode::Out;
	if (frameCount_++ >= kFadeInterval)
	{
		//シーン遷移
		controller_.ChangeScene(std::make_shared<SceneMain>(controller_));
	}
}

void SceneMain::FadeDraw()
{
	//通常の描画
	NormalDraw();

	//フェードの描画
	float rate = 0.0f;

	if (mode_ == FadeMode::In)
	{
		//黒から透明
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//透明から黒
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}

	rate = std::clamp(rate, 0.0f, 1.0f);

	//画面全体を黒にする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * rate));
	DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneMain::NormalDraw()
{
	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(FALSE);

	SetDrawMode(DX_DRAWMODE_NEAREST);
	DrawSkybox();
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	DrawGrid();

	//各クラスの描画処理
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
	for (auto& p : pScorePops_)
	{
		p.Draw();
	}
	pPlayer_->Draw();

#ifdef DEBUG_
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", frameCount_);
#endif
	char timeText[64];
	sprintf_s(timeText, "TIME LEFT : %d", (int)(kClearFadeTime - playTime_));

	DrawCenterTextWithOutline(timeText, 40, GetColor(255, 255, 255));

	//点数表示
	char scoreText[64];
	sprintf_s(scoreText, "SCORE : %d", ScoreManager::GetScore());

	DrawCenterTextWithOutline(scoreText, 90, GetColor(255, 80, 80));
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

	const int GRID_NUM = 10;      //片側のマス数
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

			//三角形2枚で四角形を表現する
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

	//三角形1
	DrawPolygon3D(&v[0], 3, tex, TRUE);

	//三角形2
	VERTEX3D v2[3] = { v[0], v[2], v[3] };
	DrawPolygon3D(v2, 3, tex, TRUE);
}

void SceneMain::DrawSkybox()
{
	float size = 800.0f;

	//カメラの位置を取得
	VECTOR cam = GetCameraPosition();

	//8頂点で立方体を表現する
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

	//背景なので光影無し
	SetUseLighting(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(FALSE);

	//各面の描画処理
	// 前
	DrawSkyQuad(v[5], v[4], v[7], v[6], skyTexture_[4]);

	// 後
	DrawSkyQuad(v[0], v[1], v[2], v[3], skyTexture_[5]);

	// 左
	DrawSkyQuad(v[4], v[0], v[3], v[7], skyTexture_[1]);

	// 右
	DrawSkyQuad(v[1], v[5], v[6], v[2], skyTexture_[0]);

	// 上
	DrawSkyQuad(v[4], v[5], v[1], v[0], skyTexture_[2]);

	// 下
	DrawSkyQuad(v[3], v[2], v[6], v[7], skyTexture_[3]);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetDrawZ(1.0f);
	SetUseLighting(TRUE);

	//printfDx("cam: %f %f %f\n", cam.x, cam.y, cam.z);
}
