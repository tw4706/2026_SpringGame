#include "SceneMain.h"
#include"../Input.h"
#include"../PopUI.h"
#include "../Game.h"
#include "ClearScene.h"
#include"../Actor/Player.h"
#include"../Actor/Enemy.h"
#include"../Physics/Camera.h"
#include"SceneController.h"
#include"../Application.h"
#include"../OperationGuideUI.h"
#include"../Manager/ScoreManager.h"
#include "../Manager/EffectManager.h"
#include"EffekseerForDXLib.h"
#include<algorithm>
#include <Dxlib.h>

namespace
{
	//敵の最大で出現する数
	constexpr int kEnemyMax = 3;
	constexpr int kMaxHP = 3;

	//HPUIのアニメーションの感覚
	constexpr float kHpAnimInterval = 0.05f;
	//HPUIを最後の画像で止めるためにそのアニメーションの最大数
	constexpr int kHpAnimMaxFrame = 11;

	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//制限時間
	constexpr float kGamePlayTime = 10.0f;

	//ゲーム開始の合図を出すフレーム数
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
	pOperationGuideUI_ = std::make_shared<OperationGuideUI>();
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

	//シャドウマップの生成
	shadowMapHandle_ = MakeShadowMap(4096, 4096);
	SetShadowMapLightDirection(shadowMapHandle_, VGet(-0.5f, -1.0f, 0.5f));
	SetShadowMapDrawArea(
		shadowMapHandle_,
		VGet(-1500, -100, -1500),
		VGet(1500, 800, 1500));

	//エフェクトのロード
	EffectManager::GetInstance().Load("hit", "data/hit.efk");
	EffectManager::GetInstance().Load("dodge", "data/justDodge.efk");
	EffectManager::GetInstance().Load("barrier", "data/barrier.efk");

	//BGM再生
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Game);

	//各クラスの初期化処理
	for (int i = 0; i < kEnemyMax; i++)
	{
		auto enemy = std::make_shared<Enemy>();
		enemy->SetPlayer(pPlayer_.get());
		enemy->SetScene(this);

		//敵をランダムな位置に配置する
		float range = 1000.0f;
		float x = ((float)rand() / RAND_MAX) * range * 2 - range;
		float z = ((float)rand() / RAND_MAX) * range * 2 - range;

		enemy->SetPos({ x, 0.0f, z });
		enemy->Init();
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

	//UIマネージャーの初期化
	uiManager_.Init();

	//操作説明クラスの初期化
	pOperationGuideUI_->Init();

	//床のモデル読み込み
	floorHandle_ = MV1LoadModel("data/floor.mv1");
	MV1SetPosition(floorHandle_, VGet(0.0f, -50.0f, 0.0f));
	MV1SetScale(floorHandle_, VGet(1.0f, 1.0f, 1.0f));

	//HPの設定
	prevHp_ = kMaxHP;
	displayHp_ = kMaxHP;

	//フレームカウントの設定
	frameCount_ = kFadeInterval;
	isGameStarted_ = false;
	remainTime_ = kGamePlayTime;
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

		if (gameStartTimer_ >= kReadyFrame + kStartFrame)
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
	if (isGameStarted_ && !isTimeUp_)
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

	if (isGameStarted_)
	{//当たり判定の処理
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
	}

	//UIの削除処理
	pPopUIs_.erase(std::remove_if(pPopUIs_.begin(), pPopUIs_.end(),
		[](const PopUI& p) { return p.IsDead(); }), pPopUIs_.end());

	//制限時間が0になったらクリアシーンへ遷移
	if (remainTime_ <= 0.0f && !isTimeUp_)
	{
		isTimeUp_ = true;
		remainTime_ = 0.0f;
	}

	//制限時間が0になってボタンが押されたらシーン遷移を行う
	if (isTimeUp_)
	{
		if (input.IsTriggered("attack"))
		{
			update_ = &SceneMain::FadeOutUpdate;
			draw_ = &SceneMain::FadeDraw;
			return;
		}
	}

	//プレイヤーが死亡したらクリアシーンに遷移
	if (pPlayer_->IsDead() && !isClearing_)
	{
		if (pPlayer_->IsDeathAnimEnd())
		{
			isClearing_ = true;
			controller_.PushScene(std::make_shared<ClearScene>(controller_));
			return;
		}
	}
}

void SceneMain::FadeOutUpdate(Input& input)
{
	frameCount_++;

	//フェード完了後に1フレーム待つ
	if (frameCount_ >= kFadeInterval && isTimeUp_)
	{
		frameCount_ = 0;
		draw_ = &SceneMain::NormalDraw;
		controller_.PushScene(std::make_shared<ClearScene>(controller_));
		return;
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

	float time = (std::max)(0.0f, remainTime_);
	int score = ScoreManager::GetDispScore();

	//UIマネージャーの描画
	uiManager_.Draw(pPlayer_->GetHP(), isHpAnimating_, damageIndex_, hpAnimFrame_,
		ScoreManager::GetBoostGauge(), time, score, isGameStarted_,
		gameStartTimer_, timeScale_, timeBonusDisplay_, timeBonusTimer_);

	//操作説明の描画
	pOperationGuideUI_->Draw();

#ifdef _DEBUG
	SetFontSize(16);
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "Frame:%d", frameCount_);
	DrawFormatString(0, 32, GetColor(255, 255, 255), "HP:%d", pPlayer_->GetHP());
#endif
}