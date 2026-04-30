#include "TitleScene.h"
#include"../Physics/Vector3.h"
#include "SceneController.h"
#include "GameScene.h"
#include"../System/Input.h"
#include"../Game.h"
#include"../System/Application.h"
#include"../GameObject/TitlePlayer.h"
#include"../Manager/EffectManager.h"
#include"EffekseerForDXLib.h"
#include<Dxlib.h>
#include<memory>
#include<algorithm>

namespace
{
	//フェードの間隔
	constexpr int kFadeInterval = 60;

	//点滅表示のデフォルトの透明度
	constexpr int kBlinkBaseAlpha = 150;

	//点滅の透明度の範囲
	constexpr int kBlinkAlphaRange = 120;

	//点滅のスピード
	constexpr float kBlinkSpeed = 0.05f;

	//ヒットエフェクトの表示座標
	const Vector3 kHitEffectOffset = { 150.0f,50.0f,0.0f };

	//カメラの座標
	const Vector3 kCameraPos = { 0.0f,0.0f,-500.0f };

	//カメラのターゲット
	const Vector3 kCameraTarget = { 0.0f,0.0f,0.0f };

	//床モデルの初期位置
	const Vector3 kFirstFloorModelPos = { -1500.0f,-250.0f,-3000.0f };

	//床モデルの角度
	constexpr float kRotateFloorModel = DX_PI_F * 0.5f;

	//床モデルのサイズ
	const float kFloorModelSize = 1.0f;

	//プレイヤーが攻撃を開始する位置
	constexpr float kPlayerAttackStart = 200.0f;

	//プレイヤーが攻撃判定を行う位置
	constexpr float kPlayerAttackHit = 120.0f;

	//敵を再生成させる位置
	constexpr float kEnemyResetPos = -500.0f;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller),
	update_(&TitleScene::FadeInUpdate),
	draw_(&TitleScene::FadeDraw),
	frameCount_(kFadeInterval),
	bgAngle_(0.0f),
	blinkTimer_(0.0f),
	titleHandle_(-1)
{
	pTitlePlayer_ = std::make_shared<TitlePlayer>();
	pTitleEnemy_ = std::make_shared<TitleEnemy>();
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	bg_.Init();

	pTitlePlayer_->Init();
	pTitleEnemy_->Init();

	SetCameraNearFar(1.0f, 10000.0f);

	//カメラ設定
	SetCameraPositionAndTarget_UpVecY(kCameraPos.ToDxlibVector(), kCameraTarget.ToDxlibVector());

	//エフェクトのロード
	EffectManager::GetInstance().Load("hit", "data/Effect/hit.efk");

	titleHandle_ = LoadGraph("data/UI/titleLogo.png");

	//床のモデル読み込み
	floorModel_.Load("data/Model/floor.mv1");
	floorModel_.SetPosition(kFirstFloorModelPos);
	floorModel_.SetRotationY(kRotateFloorModel);
	floorModel_.SetScale({ kFloorModelSize,kFloorModelSize,kFloorModelSize });


	update_ = &TitleScene::FadeInUpdate;
	draw_ = &TitleScene::FadeDraw;

	frameCount_ = kFadeInterval;

	bgAngle_ = 0.0f;
	blinkTimer_ = 0.0f;

	//BGM再生
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Title);
}

void TitleScene::Update(Input& input)
{
	(this->*update_)(input);
}

void TitleScene::Draw()
{
	(this->*draw_)();
}

void TitleScene::FadeInUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		update_ = &TitleScene::NormalUpdate;
		draw_ = &TitleScene::NormalDraw;
	}
}

void TitleScene::NormalUpdate(Input& input)
{
	blinkTimer_++;

	pTitlePlayer_->Update();
	pTitleEnemy_->Update();

	EffectManager::GetInstance().Update();
	Effekseer_Sync3DSetting();
	bgAngle_ += 0.003f;

	//距離の計算
	float dx = pTitleEnemy_->GetPos().x_ - pTitlePlayer_->GetPos().x_;

	//一定距離近づいたらプレイヤーが攻撃
	if (dx < kPlayerAttackStart && dx>0.0f)
	{
		pTitlePlayer_->Attack();
	}

	//攻撃判定
	if (dx < kPlayerAttackHit)
	{
		//ヒット時の座標保存用
		Vector3 hitPos = pTitlePlayer_->GetPos();

		//エフェクトの再生
		EffectManager::GetInstance().Play("hit", hitPos + kHitEffectOffset);

		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Hit);

		pTitleEnemy_->ReSpawn();
	}

	//画面端まで行くとリセット
	if (pTitleEnemy_->GetPos().x_ < kEnemyResetPos)
	{
		pTitleEnemy_->ReSpawn();
	}

	if (input.IsTriggered("retry"))
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);
		update_ = &TitleScene::FadeOutUpdate;
		draw_ = &TitleScene::FadeDraw;
		frameCount_ = kFadeInterval;
	}
}

void TitleScene::FadeOutUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		controller_.ChangeScene(std::make_shared<GameScene>(controller_));
	}
}

void TitleScene::FadeDraw()
{
	NormalDraw();

	float rate;

	if (update_ == &TitleScene::FadeInUpdate)
	{
		// フェードイン
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}
	rate = std::clamp(rate, 0.0f, 1.0f);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::NormalDraw()
{
	Vector3 center(0.0f, 0.0f, 0.0f);

	Vector3 cameraPos;
	cameraPos.x_ = cosf(bgAngle_) * 500.0f;
	cameraPos.y_ = 0.0f;
	cameraPos.z_ = sinf(bgAngle_) * 500.0f;

	//カメラを画面中央へ向ける
	SetCameraPositionAndTarget_UpVecY(VGet(0, 50, -500), VGet(0, 0, 0));

	SetUseBackCulling(FALSE);
	//描画
	bg_.Draw(cameraPos);

	//タイトル用プレイヤーの描画
	pTitlePlayer_->Draw();
	//タイトル用エネミーの描画
	pTitleEnemy_->Draw();

	//床のモデル描画
	floorModel_.Draw();

	SetUseBackCulling(TRUE);

	//描画
	DrawRotaGraph(Game::kScreenWidth / 2, 200, 0.6f, 0.0f, titleHandle_, true);

	//タイトル名
	const char* text = "ボタンを押してスタート";
	int textLength = (int)strlen(text);

	int totalWidth = GetDrawStringWidthToHandle(text, textLength, Game::kTitleFontHandle);
	int currentX = (Game::kScreenWidth - totalWidth) / 2;
	int baseY = Game::kScreenHeight / 2 + 200;

	int charOrder = 0;

	for (int i = 0; i < textLength; )
	{
		// --- 1文字のバイト数を判定する ---
		int byteCount = 1;

		//全角文字の対応
		// Shift-JISの場合なら,1バイト目が 0x81-0x9F か 0xE0-0xFC なら全角(2バイト)らしい
		unsigned char c = (unsigned char)text[i];
		if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) {
			byteCount = 2;
		}

		//1文字分だけを取り出す
		char oneChar[3] = { 0 };
		for (int b = 0; b < byteCount; b++) {
			oneChar[b] = text[i + b];
		}

		//文字の揺れの計算
		float waveY = sinf(blinkTimer_ * 0.1f + charOrder * 0.5f) * 10.0f;
		int drawY = static_cast<int>(baseY + waveY);

		//文字の点滅
		int alpha = static_cast<int>(kBlinkBaseAlpha + kBlinkAlphaRange * sinf(blinkTimer_ * kBlinkSpeed));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		//描画
		DrawStringToHandle(currentX + 4, drawY + 4, oneChar, GetColor(0, 0, 0), Game::kTitleFontHandle);
		DrawStringToHandle(currentX, drawY, oneChar, GetColor(255, 255, 255), Game::kTitleFontHandle);

		//次の文字へ進む
		currentX += GetDrawStringWidthToHandle(oneChar, byteCount, Game::kTitleFontHandle);
		i += byteCount;
		charOrder++;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}