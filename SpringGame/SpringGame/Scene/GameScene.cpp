#include "GameScene.h"
#include "../Game.h"
#include "ResultScene.h"
#include"../UI/PopUI.h"
#include"../System/Input.h"
#include"SceneController.h"
#include"../GameObject/Player.h"
#include"../GameObject/Enemy.h"
#include"../Physics/Camera.h"
#include"../System/Application.h"
#include"../UI/OperationGuideUI.h"
#include"../GameObject/GoalObject.h"
#include "../Manager/EffectManager.h"
#include "../GameObject/EnemySpawner.h"
#include"EffekseerForDXLib.h"
#include<algorithm>
#include <Dxlib.h>

namespace
{
	//敵の最大で出現する数
	constexpr int kEnemyMax = 3;

	//敵を生成するスポナーの設置数
	constexpr int kEnemySpawnerNum = 3;

	//ウェーブの最大数
	constexpr int kMaxWave = 5;

	//最大体力
	constexpr int kMaxHP = 3;

	//HPUIのアニメーションの感覚
	constexpr float kHpAnimInterval = 0.05f;
	//HPUIを最後の画像で止めるためにそのアニメーションの最大数
	constexpr int kHpAnimMaxFrame = 11;

	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//制限時間
	constexpr float kGamePlayTime = 60.0f;

	//ゲーム開始の合図を出すフレーム数
	constexpr int kReadyFrame = 60;
	constexpr int kStartFrame = 30;

	//敵の生成範囲(X座標・Z座標)
	constexpr float kWalkXLimit = 130.0f;
	constexpr float kWalkZLimit = 10000.0f;

	//敵スポナーの生成Z座標
	constexpr float kEnemySpawnerPosZ = 1500.0f;

	//スポナーの生成間隔Z座標
	constexpr float kEnemySpawnerDistanceZ = 3000.0f;

	//敵スポナーの半径
	constexpr float kEnemySpawnerRadius = 1000.0f;

	//床モデルの初期位置
	const Vector3 kFirstFloorPos = { 0.0f,-50.0f,3000.0f };

	//経過時間
	constexpr float kDeltaTime = 1.0f / 60.0f;

	//スローの倍率
	constexpr float kTimeScale = 0.4f;

	//スロー時間
	constexpr float kSlowTime = 0.2f;

	//カメラのズーム倍率
	constexpr float kCameraZoom = DX_PI_F / 6.0f;
}

GameScene::GameScene(SceneController& contorller) :
	Scene(contorller),
	frameCount_(0),
	hpHandle_(-1),
	floorHandle_(-1),
	clearTime_(0.0f),
	timeScale_(1.0f),
	slowTimer_(0.0f),
	dt_(0.0f),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw)
{
	pOperationGuideUI_ = std::make_shared<OperationGuideUI>();
	pPlayer_ = std::make_shared<Player>();
	pCamera_ = std::make_shared<Camera>();
	pGoalObject_ = std::make_shared<GoalObject>();
}

GameScene::~GameScene()
{

}

void GameScene::Init()
{
	//カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(true);

	//Zバッファの設定
	SetUseZBuffer3D(true);		//Zバッファを使います
	SetWriteZBuffer3D(true);	//描画する物体はZバッファにも距離を書き込む

	//シャドウマップの生成
	shadowMapHandle_ = MakeShadowMap(4096, 4096);
	SetShadowMapLightDirection(shadowMapHandle_, VGet(-0.5f, -1.0f, 0.5f));
	SetShadowMapDrawArea(shadowMapHandle_,
		VGet(-1500, -100, -3000), VGet(4500, 800, 10000));

	//エフェクトのロード
	EffectManager::GetInstance().Load("hit", "data/Effect/hit.efk");
	EffectManager::GetInstance().Load("dodge", "data/Effect/justDodge.efk");
	EffectManager::GetInstance().Load("barrier", "data/Effect/barrier.efk");
	EffectManager::GetInstance().Load("spawn", "data/Effect/enemySpawn.efk");
	EffectManager::GetInstance().Load("areaLock", "data/Effect/areaLock.efk");
	EffectManager::GetInstance().Load("enemyAttack", "data/Effect/enemyAttack.efk");

	//BGM再生
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Game);

	//各クラスの初期化処理
	//敵の生成
	for (int i = 0; i < kEnemySpawnerNum; i++)
	{
		auto spawner = std::make_shared<EnemySpawner>();

		float z = kEnemySpawnerPosZ + i * kEnemySpawnerDistanceZ;

		spawner->Init({ 0.0f, 0.0f, z }, kEnemySpawnerRadius);
		spawner->SetPlayer(pPlayer_.get());
		spawner->SetCamera(pCamera_.get());

		pEnemySpawner_.push_back(spawner);
	}

	prevLocked_.resize(pEnemySpawner_.size());

	for (int i = 0; i < pEnemySpawner_.size(); i++)
	{
		prevLocked_[i] = false;
	}

	//プレイヤーの初期化
	pPlayer_->Init();
	pPlayer_->SetCamera(pCamera_.get());

	//カメラの初期化
	pCamera_->SetPlayer(pPlayer_);
	pCamera_->Init();

	//ゴールオブジェクトの初期化
	pGoalObject_->Init();

	//背景の初期化
	bg_.Init();

	//UIマネージャーの初期化
	gameSceneUI_.Init();

	//操作説明クラスの初期化
	pOperationGuideUI_->Init();

	//床のモデル読み込み
	floorHandle_ = MV1LoadModel("data/floor.mv1");
	MV1SetPosition(floorHandle_, kFirstFloorPos.ToDxlibVector());
	MV1SetScale(floorHandle_, VGet(1.0f, 1.0f, 1.0f));

	//HPの設定
	prevHp_ = kMaxHP;
	displayHp_ = kMaxHP;

	//フレームカウントの設定
	frameCount_ = kFadeInterval;
	isGameStarted_ = false;
	clearTime_ = 0.0f;
}

void GameScene::Update(Input& input)
{
	dt_ = kDeltaTime * timeScale_;

	EffectManager::GetInstance().Update();
	(this->*update_)(input);
}

void GameScene::Draw()
{
	(this->*draw_)();
}

void GameScene::AddScorePop(const Vector3& pos, int value)
{
	pPopUIs_.emplace_back(pos, value, ScorePopType::Score);
}

void GameScene::DrawCenterTextWithOutline(const char* text, int y, int color, int screenW)
{
	int width = GetDrawStringWidth(text, static_cast<int>(strlen(text)));
	int x = (screenW - width) / 2;

	//描画
	DrawString(x, y, text, color);
}

void GameScene::FadeInUpdate(Input& input)
{
	//空のインプットを渡す（プレイヤーは操作できないが、アニメーションなどは更新するため）
	Input emptyInput;
	pPlayer_->Update(emptyInput, dt_);
	pCamera_->Update();
	Effekseer_Sync3DSetting();

	if (frameCount_-- <= 0)
	{
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
	}
}

void GameScene::NormalUpdate(Input& input)
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
		clearTime_ += dt_;
	}

	//ポップするスコアの更新処理
	for (auto& p : pPopUIs_)
	{
		p.Update(dt_);
	}

	//敵スポナーの更新
	for (auto& spawner : pEnemySpawner_)
	{
		spawner->Update(pPlayer_->GetPos(), dt_);
	}

	EnemySpawner* lockedSpawner = nullptr;

	//1つだけ取得
	for (auto& spawner : pEnemySpawner_)
	{
		if (spawner->IsLocked())
		{
			lockedSpawner = spawner.get();
			break;
		}
	}

	for (int i = 0; i < pEnemySpawner_.size(); i++)
	{
		bool nowLocked = pEnemySpawner_[i]->IsLocked();

		if (prevLocked_[i] && !nowLocked)
		{
			currentWave_++;
		}

		prevLocked_[i] = nowLocked;
	}

	if (lockedSpawner)
	{
		Vector3 playerPos = pPlayer_->GetPos();
		Vector3 center = lockedSpawner->GetPos();

		float dx = playerPos.x_ - center.x_;
		float dz = playerPos.z_ - center.z_;

		float distSq = dx * dx + dz * dz;
		float radius = lockedSpawner->GetRadius();

		if (distSq > radius * radius)
		{
			float dist = sqrtf(distSq);

			dx /= dist;
			dz /= dist;

			playerPos.x_ = center.x_ + dx * radius;
			playerPos.z_ = center.z_ + dz * radius;

			pPlayer_->SetPos(playerPos);
		}
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

	//ゴールオブジェクトの更新
	pGoalObject_->Update();

	//ゴールに触れたら
	if (pGoalObject_->IsHit())
	{
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frameCount_ = 0;
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
		timeScale_ = kTimeScale;					//スローの倍率
		slowTimer_ = kSlowTime;						//スロー時間
		pCamera_->StartZoom(kCameraZoom);			//カメラのズーム開始
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

	if (isGameStarted_)
	{
		//当たり判定の初期化
		collisionManager_.Clear();

		//当たり判定の登録
		collisionManager_.AddCollider(pPlayer_->GetCollider());
		collisionManager_.AddCollider(pPlayer_->GetAttackCollider());
		collisionManager_.AddCollider(pGoalObject_->GetCollider());

		for (auto& spawner : pEnemySpawner_)
		{
			for (auto& enemy : spawner->GetEnemy())
			{
				collisionManager_.AddCollider(enemy->GetCollider());
			}
		}

		//すべてのコライダーの判定
		collisionManager_.CheckAllCollision();
	}

	//UIの削除処理
	pPopUIs_.erase(std::remove_if(pPopUIs_.begin(), pPopUIs_.end(),
		[](const PopUI& p) { return p.IsDead(); }), pPopUIs_.end());

	//プレイヤーが死亡したらクリアシーンに遷移
	if (pPlayer_->IsDead() && !isClearing_)
	{
		if (pPlayer_->IsDeathAnimEnd())
		{
			isClearing_ = true;

			for (auto& spawner : pEnemySpawner_)
			{
				spawner->StopEffect();
			}

			controller_.PushScene(std::make_shared<ResultScene>(controller_, clearTime_, ResultTextType::GameOver));
			return;
		}
	}
}

void GameScene::FadeOutUpdate(Input& input)
{
	frameCount_++;

	//ゴールオブジェクトに触れたら
	if (pGoalObject_->IsHit())
	{
		controller_.PushScene(std::make_shared<ResultScene>(controller_, clearTime_,ResultTextType::Clear));
		return;
	}
}

void GameScene::FadeDraw()
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

void GameScene::NormalDraw()
{
	//シャドウマップ描画
	ShadowMap_DrawSetup(shadowMapHandle_);

	//プレイヤーと敵を影で使用するため影用に描画
	pPlayer_->Draw();

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

	//スコアの描画
	for (auto& p : pPopUIs_)
	{
		p.Draw();
	}

	//敵スポナーの描画
	for (auto& spawner : pEnemySpawner_)
	{
		//スポナーの描画
		spawner->Draw();

		for (auto& enemy : spawner->GetEnemy())
		{
			//敵の描画
			enemy->Draw();
		}
	}

	//プレイヤーの描画
	pPlayer_->Draw();

	//ゴールオブジェクトの描画
	pGoalObject_->Draw();

	float time = (std::max)(0.0f, clearTime_);

	//UIマネージャーの描画
	gameSceneUI_.Draw(pPlayer_->GetHP(), isHpAnimating_, damageIndex_, hpAnimFrame_, time, isGameStarted_,
		gameStartTimer_, timeScale_, currentWave_);

	//操作説明の描画
	pOperationGuideUI_->Draw();

#ifdef _DEBUG
	SetFontSize(16);
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));
	DrawFormatString(0, 16, GetColor(255, 255, 255), "Frame:%d", frameCount_);
	DrawFormatString(0, 32, GetColor(255, 255, 255), "HP:%d", pPlayer_->GetHP());

	//現在ステージ上にいる敵の数
	int totalEnemyCount = 0;

	for (auto& spawner : pEnemySpawner_)
	{
		totalEnemyCount += (int)spawner->GetEnemy().size();
	}

	DrawFormatString(20, 200, GetColor(255, 255, 255),
		"Total Enemy : %d", totalEnemyCount);
#endif
}
