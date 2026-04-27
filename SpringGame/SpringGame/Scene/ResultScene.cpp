#include "ResultScene.h"
#include "TitleScene.h"
#include"SceneController.h"
#include"GameScene.h"
#include"../System/Input.h"
#include"../Game.h"
#include"../System/Application.h"
#include <algorithm>
#include<Dxlib.h>

namespace
{
	//フェードの感覚
	constexpr int kFadeInterval = 60;

	//スコアテキストのオフセットY座標
	constexpr int kscoreTextOffsetY = 100;

	//点滅の時間
	constexpr float kBlinkTime = 0.01f;

	//背景の回転速度
	constexpr float kBgAngleSpeed = 0.01f;

	//TIME表示のY座標オフセット
	constexpr int kTimeTextOffsetY = 100;

	//選択肢のY座標オフセット
	constexpr int kMenuOffsetY = 50;

	//resultの結果の文字表示のY座標オフセット
	constexpr int kResultTextOffsetY = 200;

	//Resultの結果文字のサイズ
	constexpr float kResultTextScale = 1.2f;
}

ResultScene::ResultScene(SceneController& controller, float clearTime, ResultTextType resultType) :
	Scene(controller),
	clearTime_(clearTime),
	update_(&ResultScene::FadeInUpdate),
	draw_(&ResultScene::FadeDraw),
	frameCount_(kFadeInterval),
	resultScore_(0),
	displayScore_(0),
	bgAngle_(0.0f),
	resultType_(resultType)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init()
{
	update_ = &ResultScene::FadeInUpdate;
	draw_ = &ResultScene::FadeDraw;

	frameCount_ = kFadeInterval;

	//BGM再生
	Application::GetInstance().GetSoundManager().PlayBgm(BGM::Result);
}

void ResultScene::Update(Input& input)
{
	(this->*update_)(input);
}
void ResultScene::Draw()
{
	(this->*draw_)();
}

void ResultScene::FadeInUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		update_ = &ResultScene::NormalUpdate;
		draw_ = &ResultScene::NormalDraw;
	}
}

void ResultScene::NormalUpdate(Input& input)
{
	frameCount_++;
	blinkTimer_ += kBlinkTime;

	//背景を回転させる
	bgAngle_ += kBgAngleSpeed;

	if (frameCount_ >= kFadeInterval)
	{
		if (displayScore_ < resultScore_)
		{
			int diff = resultScore_ - displayScore_;
			displayScore_ += (std::max)(1, diff / 10);

			if (displayScore_ > resultScore_)
			{
				displayScore_ = resultScore_;
			}
		}
	}

	//選択を選ぶ
	if (input.IsTriggered("up") || input.IsTriggered("down"))
	{
		if (currentMenu_ == ResultMenu::Retry)
		{
			currentMenu_ = ResultMenu::Title;
		}
		else
		{
			currentMenu_ = ResultMenu::Retry;
		}
	}

	//リトライが行われたら
	if (input.IsTriggered("retry"))
	{
		//SE再生
		Application::GetInstance().GetSoundManager().PlaySe(SE::Decide);

		update_ = &ResultScene::FadeOutUpdate;
		draw_ = &ResultScene::FadeDraw;

		frameCount_ = kFadeInterval;
	}
}

void ResultScene::FadeOutUpdate(Input& input)
{
	if (frameCount_-- <= 0)
	{
		if (currentMenu_ == ResultMenu::Retry)
		{
			controller_.ResetScene(std::make_shared<GameScene>(controller_));
			return;
		}
		else
		{
			controller_.ResetScene(std::make_shared<TitleScene>(controller_));
			return;
		}
	}
}

void ResultScene::FadeDraw()
{
	//通常の描画
	NormalDraw();

	float rate;

	if (update_ == &ResultScene::FadeInUpdate)
	{
		//フェードイン
		rate = (float)frameCount_ / kFadeInterval;
	}
	else
	{
		//フェードアウト
		rate = 1.0f - (float)frameCount_ / kFadeInterval;
	}
	rate = std::clamp(rate, 0.0f, 1.0f);

	//黒画面から透明
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * rate));
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ResultScene::NormalDraw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,180);
	DrawBoxAA(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//表示用
	int timeInt = (int)clearTime_;
	int timeDec = (int)((clearTime_ - timeInt) * 10);

	char buf[64];
	sprintf_s(buf, "TIME : %d.%d", timeInt, timeDec);

	//文字幅取得
	int width = GetDrawStringWidthToHandle(buf, strlen(buf), Game::kFontUIHandle);

	//表示する座標
	int x = Game::kScreenWidth / 2 - width / 2;
	int y = Game::kScreenHeight / 2 - kTimeTextOffsetY;

	//描画
	DrawStringToHandle(x, y, buf, GetColor(255, 255, 255), Game::kFontUIHandle);

	//選択肢
	const char* retryText = "リトライ";
	const char* titleText = "タイトルへ戻る";

	//表示座標
	int centerX = Game::kScreenWidth / 2;
	int baseY = Game::kScreenHeight / 2 + kMenuOffsetY;

	//拡大縮小
	float retryScale = (currentMenu_ == ResultMenu::Retry) ? 1.0f + 0.1f * sinf(blinkTimer_ * 10.0f) : 1.0f;
	float titleScale = (currentMenu_ == ResultMenu::Title) ? 1.0f + 0.1f * sinf(blinkTimer_ * 10.0f) : 1.0f;

	//文字幅取得
	int retryW = GetDrawStringWidthToHandle(retryText, strlen(retryText), Game::kFontUIHandle);
	int titleW = GetDrawStringWidthToHandle(titleText, strlen(titleText), Game::kFontUIHandle);

	//テキストの色
	int resultColor = GetColor(255, 255, 255);
	int titleColor = GetColor(255, 255, 255);

	//選ばれている場合は色を変更
	if (currentMenu_ == ResultMenu::Retry)
	{
		resultColor = GetColor(255, 0, 0);
	}
	else
	{
		titleColor= GetColor(255, 0, 0);
	}

	//描画
	DrawExtendStringToHandle(
		centerX - (int)(retryW * retryScale / 2),
		baseY,
		retryScale,
		retryScale,
		retryText,
		resultColor,
		Game::kFontUIHandle);

	DrawExtendStringToHandle(
		centerX - (int)(titleW * titleScale / 2),
		baseY + kMenuOffsetY,
		titleScale,
		titleScale,
		titleText,
		titleColor,
		Game::kFontUIHandle);

	//ゲームオーバーまたはゲームクリアの表示
	const char* resultText = "";
	int resultTextColor = GetColor(255, 255, 255);

	if (resultType_ == ResultTextType::Clear)
	{
		resultText = "GAME CLEAR！";
		resultTextColor = GetColor(255, 255, 255);
	}
	else
	{
		resultText = "GAME OVER...";
		resultTextColor = GetColor(255, 255, 255);
	}

	//文字の幅を取得して中央に配置
	int resWidth = GetDrawStringWidthToHandle(resultText, (int)strlen(resultText), Game::kFontUIHandle);
	int resultX = Game::kScreenWidth / 2 - resWidth / 2;
	int resultY = Game::kScreenHeight / 2 - kResultTextOffsetY;

	//描画
	DrawExtendStringToHandle(resultX+4, resultY+4, kResultTextScale, kResultTextScale, resultText, GetColor(0,0,0), Game::kFontUIHandle);
	DrawExtendStringToHandle(resultX, resultY, kResultTextScale, kResultTextScale, resultText, resultTextColor, Game::kFontUIHandle);
}