#include "SceneMain.h"
#include"../Actor/Player.h"
#include"../Actor/Enemy.h"
#include"../Input.h"
#include"../Physics/Camera.h"
#include"../ScoreManager.h"
#include"../PopUI.h"
#include"SceneController.h"
#include "ClearScene.h"
#include "../EffectManager.h"
#include"EffekseerForDXLib.h"
#include<algorithm>
#include <Dxlib.h>

namespace
{
	//敵の最大で出現する数
	constexpr int kEnemyMax = 3;

	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//制限時間
	constexpr float kClearFadeTime = 60.0f;
}

SceneMain::SceneMain(SceneController& contorller) :
	Scene(contorller),
	frameCount_(0),
	playTime_(0.0f),
	bonusTime_(0.0f),
	timeScale_(1.0f),
	slowTimer_(0.0f),
	dt_(0.0f),
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
	SetFontSize(40);


	//エフェクトのロード
	EffectManager::GetInstance().Load("hit", "data/hit.efk");
	EffectManager::GetInstance().Load("dodge", "data/justDodge.efk");

	//各クラスの初期化処理
	for (int i = 0; i < kEnemyMax; i++)
	{
		auto enemy = std::make_shared<Enemy>();
		enemy->SetPlayer(pPlayer_.get());
		enemy->SetScene(this);
		enemy->Init();

		//敵をランダムな位置に配置する
		float range = 1000.0f;
		float x = ((float)rand() / RAND_MAX) * range * 2 - range;
		float z = ((float)rand() / RAND_MAX) * range * 2 - range;

		enemy->SetPos({ x, 0.0f, z });

		enemies_.push_back(enemy);
	}

	pPlayer_->Init();
	pPlayer_->SetCamera(pCamera_.get());
	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();

	bg_.Init();

	frameCount_ = kFadeInterval;
	dt_ = (1.0f / 60.0f) * timeScale_;
}

void SceneMain::Update(Input& input)
{
	dt_ = (1.0f / 60.0f) * timeScale_;
	EffectManager::GetInstance().Update();
	(this->*update_)(input);
}

void SceneMain::Draw()
{
	(this->*draw_)();
}

void SceneMain::AddScorePop(const Vector3& pos, int value)
{
	pPopUIs_.emplace_back(pos, value, ScorePopType::Score);
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
	for (auto& enemy : enemies_)
	{
		enemy->Update(dt_);
	}
	pPlayer_->Update(input, dt_);
	pCamera_->Update();
	Effekseer_Sync3DSetting();

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
	playTime_ += dt_;

	//スコアの更新処理
	ScoreManager::Update();

	//各クラスの更新処理
	for (auto& enemy : enemies_)
	{
		enemy->Update(dt_);
	}

	for (auto& p : pPopUIs_)
	{
		p.Update(dt_);
	}
	pPlayer_->Update(input, dt_);
	pCamera_->Update();
	Effekseer_Sync3DSetting();

	if (pPlayer_->ConsumeJustDodge())
	{
		timeScale_ = 0.4f;   //スロー倍率
		slowTimer_ = 0.2f;   //スロー時間
		pCamera_->StartZoom(DX_PI_F / 6.0f);

		//ジャスト回避成功時時間を増やす
		bonusTime_ += 2.0f;

		timeBonusDisplay_ = 2.0f;
		timeBonusTimer_ = 1.0f;
	}
	//スロー時間の更新
	if (slowTimer_ > 0.0f)
	{
		slowTimer_ -= dt_;

		if (slowTimer_ <= 0.0f)
		{
			timeScale_ = 1.0f;
		}
	}
	if (timeBonusTimer_ > 0.0f)
	{
		timeBonusTimer_ -= dt_;
	}

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
	pPopUIs_.erase(
		std::remove_if(pPopUIs_.begin(), pPopUIs_.end(),
			[](const PopUI& p) { return p.IsDead(); }),
		pPopUIs_.end());

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
	if (playTime_ >= kClearFadeTime&&!isClearing_)
	{
		//プレイヤーの死亡アニメーション終了でクリアシーンへ
		if (pPlayer_->IsDeathAnimEnd())
		{
			isClearing_ = true;

			update_ = &SceneMain::FadeOutUpdate;
			draw_ = &SceneMain::FadeDraw;

			frameCount_ = 0;
		}
		return;
	}

	//プレイヤーが死亡したらクリアシーンに遷移
	if (pPlayer_->IsDead() && !isClearing_)
	{
		isClearing_ = true;

		update_ = &SceneMain::FadeOutUpdate;
		draw_ = &SceneMain::FadeDraw;

		frameCount_ = 0;
		return;
	}
}

void SceneMain::FadeOutUpdate(Input& input)
{
	frameCount_++;

	// フェード完了後に1フレーム待つ
	if (frameCount_ >= kFadeInterval)
	{
		frameCount_ = 0;
		draw_ = &SceneMain::NormalDraw;
		controller_.PushScene(std::make_shared<ClearScene>(controller_));
	}
}

void SceneMain::FadeDraw()
{
	//通常の描画
	NormalDraw();

	//フェードの描画
	float rate = 0.0f;
	rate = (float)frameCount_ / kFadeInterval;

	rate = std::clamp(rate, 0.0f, 1.0f);

	//画面全体を黒にする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * rate));
	DrawBoxAA(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneMain::NormalDraw()
{
	bg_.Draw(pCamera_->GetPos());
	DrawGrid();

	//敵の描画
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}

	//スコアの描画
	for (auto& p : pPopUIs_)
	{
		p.Draw();
	}

	pPlayer_->Draw();


	if (timeScale_ < 1.0f)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(0, 0, 1280, 720, GetColor(180, 100, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

#ifdef _DEBUG
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", frameCount_);

	DrawFormatString(0, 32, GetColor(255, 255, 255), "HP:%d", pPlayer_->GetHP());
#endif
	int time = (int)(kClearFadeTime - playTime_ + bonusTime_);
	int score = ScoreManager::GetDispScore();

	uiManager_.Draw(time,score,timeBonusDisplay_,timeBonusTimer_);
}

void SceneMain::DrawGrid()
{
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