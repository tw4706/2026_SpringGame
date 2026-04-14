#include "SceneMain.h"
#include"../Actor/Player.h"
#include"../Actor/Enemy.h"
#include"../Input.h"
#include"../Physics/Camera.h"
#include"../Manager/ScoreManager.h"
#include"../PopUI.h"
#include"SceneController.h"
#include "ClearScene.h"
#include "../Game.h"
#include "../Manager/EffectManager.h"
#include"EffekseerForDXLib.h"
#include<algorithm>
#include <Dxlib.h>

namespace
{
	//敵の最大で出現する数
	constexpr int kEnemyMax = 3;
	constexpr int kMaxHP = 3;

	constexpr float kHpAnimInterval = 0.05f;
	constexpr int kHpAnimMaxFrame = 11;

	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//制限時間
	constexpr float kClearFadeTime = 60.0f;

	constexpr int kFrameW = 32;
	constexpr int kFrameH = 32;
	constexpr float kHpScale = 2.0f;

	//片側のマス数
	constexpr int kGridNum = 10;
	constexpr float kTileSize = 100.0f;

	constexpr int kReadyFrame = 60;
	constexpr int kStartFrame = 30;
}

SceneMain::SceneMain(SceneController& contorller) :
	Scene(contorller),
	frameCount_(0),
	hpHandle_(-1),
	floorHandle_(-1),
	remainTime_(0.0f),
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
	SetUseBackCulling(true);

	//Zバッファの設定
	SetUseZBuffer3D(true);		//Zバッファを使います
	SetWriteZBuffer3D(true);	//描画する物体はZバッファにも距離を書き込む

	//HPバーの画像のロード
	hpHandle_ = LoadGraph("data/HP.png");

	//シャドウマップの生成
	shadowMapHandle_ = MakeShadowMap(4096, 4096);
	SetShadowMapLightDirection(shadowMapHandle_, VGet(-0.5f, -1.0f, 0.5f));
	SetShadowMapDrawArea(
		shadowMapHandle_,
		VGet(-1500, -100, -1500),
		VGet(1500, 800, 1500));

	prevHp_ = kMaxHP;
	displayHp_ = kMaxHP;

	//エフェクトのロード
	EffectManager::GetInstance().Load("hit", "data/hit.efk");
	EffectManager::GetInstance().Load("dodge", "data/justDodge.efk");
	EffectManager::GetInstance().Load("barrier", "data/barrier.efk");

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

	//プレイヤーの初期化
	pPlayer_->Init();
	pPlayer_->SetCamera(pCamera_.get());

	//カメラの初期化
	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();

	//背景の初期化
	bg_.Init();

	//床のモデル読み込み
	floorHandle_ = MV1LoadModel("data/floor.mv1");
	MV1SetPosition(floorHandle_, VGet(0, -50, 0));
	MV1SetScale(floorHandle_, VGet(1.0f, 1.0f, 1.0f));

	frameCount_ = kFadeInterval;
	dt_ = (1.0f / 60.0f) * timeScale_;
	isGameStarted_ = false;
	remainTime_ = kClearFadeTime;
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
	int width = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	int x = (screenW - width) / 2;

	//描画
	DrawString(x, y, text, color);
}

void SceneMain::FadeInUpdate(Input& input)
{
	for (auto& enemy : enemies_)
	{
		enemy->Update(dt_);
	}

	//空のインプットを渡す（プレイヤーは操作できないが、アニメーションなどは更新するため）
	Input emptyInput;
	pPlayer_->Update(emptyInput, dt_);
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

	if (!isGameStarted_)
	{
		gameStartTimer_++;

		if (gameStartTimer_ > kReadyFrame + kStartFrame)
		{
			isGameStarted_ = true;
		}
	}

	if (isGameStarted_)
	{
		//時間の更新
		remainTime_ -= dt_;

		//スコアの更新処理
		ScoreManager::Update(dt_);

		//各クラスの更新処理
		for (auto& enemy : enemies_)
		{
			enemy->Update(dt_);
		}
	}

	//ポップするスコアの更新処理
	for (auto& p : pPopUIs_)
	{
		p.Update(dt_);
	}

	//プレイヤーとカメラの更新
	if (isGameStarted_)
	{
		pPlayer_->Update(input, dt_);
	}
	else
	{
		//空のインプットを渡す（プレイヤーは操作できないが、アニメーションなどの更新をするため）
		Input emptyInput;
		pPlayer_->Update(emptyInput, dt_);
	}
	pCamera_->Update();
	Effekseer_Sync3DSetting();

	int currentHp = pPlayer_->GetHP();

	//ダメージ受けた瞬間にHPUIのアニメーション開始
	if (currentHp < prevHp_)
	{
		isHpAnimating_ = true;
		hpAnimFrame_ = 0;
		hpAnimTimer_ = 0.0f;

		damageIndex_ = prevHp_ - 1;
	}

	prevHp_ = currentHp;

	//アニメ更新
	if (isHpAnimating_)
	{
		hpAnimTimer_ += dt_;

		if (hpAnimTimer_ >= kHpAnimInterval)
		{
			hpAnimTimer_ = 0.0f;
			hpAnimFrame_++;

			if (hpAnimFrame_ >= kHpAnimMaxFrame)
			{
				hpAnimFrame_ = kHpAnimMaxFrame - 1; //最後で止める
				hpKeepFrame_ = hpAnimFrame_;
				isHpAnimating_ = false;
				displayHp_ = currentHp;
			}
			else
			{
				//HPが減る前のHPを表示するフレームを一定時間キープする
				hpKeepFrame_ = hpAnimFrame_;
			}
		}
	}

	//ジャスト回避処理
	if (pPlayer_->IsJustDodge())
	{
		timeScale_ = 0.4f;						//スローの倍率
		slowTimer_ = 0.2f;						//スロー時間
		pCamera_->StartZoom(DX_PI_F / 6.0f);	//カメラのズーム開始

		//ジャスト回避成功時に制限時間を増やす
		remainTime_ += 2.0f;
		ScoreManager::AddScoreBoostTime();

		timeBonusDisplay_ = 2.0f;
		timeBonusTimer_ = 1.0f;
	}

	//スロー時間の更新
	if (slowTimer_ > 0.0f)
	{
		//スロー中は時間にタイムスケールをかけて遅くする
		slowTimer_ -= dt_;

		if (slowTimer_ <= 0.0f)
		{
			//スロー終了
			timeScale_ = 1.0f;
		}
	}
	//時間ボーナスの更新
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

	//判定
	collisionManager_.CheckAllCollision();

	//敵の削除処理
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::shared_ptr<Enemy>& e)
			{
				return e->IsDestroy();
			}),
		enemies_.end());

	//UIの削除処理
	pPopUIs_.erase(std::remove_if(pPopUIs_.begin(), pPopUIs_.end(),
		[](const PopUI& p) { return p.IsDead(); }), pPopUIs_.end());

	//常時何体か湧いているようにする
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

	//制限時間が0になったらクリアシーンへ遷移
	if (remainTime_ <= 0.0f && !isClearing_)
	{
		isClearing_ = true;

		controller_.PushScene(std::make_shared<ClearScene>(controller_));
		return;
	}

	//プレイヤーが死亡したらクリアシーンに遷移
	if (pPlayer_->IsDead() && !isClearing_)
	{
		if (pPlayer_->IsDeathAnimEnd())
		{
			isClearing_ = true;

			controller_.PushScene(std::make_shared<ClearScene>(controller_));
		}
		return;
	}
}

void SceneMain::FadeOutUpdate(Input& input)
{
	frameCount_++;

	//フェード完了後に1フレーム待つ
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
	//シャドウマップ描画
	ShadowMap_DrawSetup(shadowMapHandle_);

	//プレイヤーと敵を影で使用するため影用に描画
	pPlayer_->Draw();

	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}

	//シャドウマップ描画終了
	ShadowMap_DrawEnd();

	//3D描画を初期化
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	SetUseBackCulling(false);

	//背景の描画
	bg_.Draw(pCamera_->GetPos());
	SetUseShadowMap(0, shadowMapHandle_);
	MV1DrawModel(floorHandle_);
	SetUseShadowMap(0, -1);

	SetUseBackCulling(true);

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

	//プレイヤーの描画
	pPlayer_->Draw();

	//ジャスト回避時に画面の色を変更
	if (timeScale_ < 1.0f)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(0, 0, 1280, 720, GetColor(180, 100, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//HPUI描画
	//現在のプレイヤーのHP取得
	int hp = pPlayer_->GetHP();

	int x = 50;
	int y = 50;

	//拡大後サイズ
	int drawW = static_cast<int>(kFrameW * kHpScale);
	int drawH = static_cast<int>(kFrameH * kHpScale);

	//最大3このハートを描画する
	const int MAX_HP = 3;

	for (int i = 0; i < MAX_HP; i++)
	{
		int drawX = x + i * (drawW + 10);

		//中心座標を指定する
		int centerX = drawX + drawW / 2;
		int centerY = y + drawH / 2;

		int srcX = 0;

		if (isHpAnimating_ && i == damageIndex_)srcX = hpAnimFrame_ * kFrameW;
		else if (i < hp)srcX = 0;
		else srcX = (kHpAnimMaxFrame - 1) * kFrameW;

		DrawRectRotaGraph(centerX, centerY,
			srcX, 192,
			kFrameW, kFrameH,
			kHpScale, 0.0,
			hpHandle_, TRUE, FALSE);
	}

#ifdef _DEBUG
	SetFontSize(16);
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "Frame:%d", frameCount_);
	DrawFormatString(0, 32, GetColor(255, 255, 255), "HP:%d", pPlayer_->GetHP());
#endif
	SetFontSize(40);
	int time = (std::max)(0, (int)(remainTime_));
	int score = ScoreManager::GetDispScore();

	uiManager_.Draw(time, score, timeBonusDisplay_, timeBonusTimer_);

	float boostGauge = ScoreManager::GetBoostGauge();

	if (boostGauge > 0.0f)
	{
		int barX = 50;
		int barY = 110;
		int barW = 220;
		int barH = 18;

		int currentW = (std::max)(4, static_cast<int>(barW * boostGauge));

		//外枠
		DrawBox(barX, barY, barX + barW, barY + barH,
			GetColor(255, 255, 255), FALSE);

		//中身
		DrawBox(barX + 2, barY + 2,
			barX + currentW - 2, barY + barH - 2,
			GetColor(255, 220, 0), TRUE);

		//x2表示
		DrawFormatString(barX + barW + 10, barY - 4,
			GetColor(255, 220, 0), "x2");
	}

	//開始の合図の描画
	if (!isGameStarted_)
	{
		//スタートタイマーが60フレームより小さい場合はReadyそれ以降はGo!
		if (gameStartTimer_ < kReadyFrame)
		{
			DrawCenterTextWithOutline("READY",Game::kScreenHeight / 2,GetColor(255, 255, 0),Game::kScreenWidth);
		}
		else
		{
			DrawCenterTextWithOutline("Go!",Game::kScreenHeight / 2,GetColor(0, 255, 255),Game::kScreenWidth);
		}
	}
}