#include "Application.h"
#include "../Game.h"
#include "Input.h"
#include "../Scene/TitleScene.h"
#include "../Scene/SceneController.h"
#include"../SoundManager.h"
#include "EffekseerForDXLib.h"
#include<Dxlib.h>

Application::Application()
{

}

Application::~Application()
{
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	// ウィンドウモード設定
	ChangeWindowMode(true);
	// ウィンドウのタイトル変更
	SetMainWindowText("ゲーム名");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK);

	//------------------------------//
	// エフェクトの初期化
	//------------------------------//
	{
		// DirectX9を使用するようにする。(DirectX11も可)
		// Effekseerを使用するには必ず設定する。
		SetUseDirect3DVersion(DX_DIRECT3D_11);

		// Effekseerを初期化する。
		// 引数には画面に表示する最大パーティクル数を設定する。
		if (Effkseer_Init(3000) == -1)
		{
			DxLib_End();
			return false;
		}

		// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
		// Effekseerを使用する場合は必ず設定する。
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

		// DXライブラリのデバイスロストした時のコールバックを設定する。
		// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
		// ただし、DirectX11を使用する場合は実行する必要はない。
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();

		// Effekseerに2D描画の設定をする。
		Effekseer_Set2DSetting(Game::kScreenWidth, Game::kScreenHeight);

		// Zバッファを有効にする。
		// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
		SetUseZBuffer3D(TRUE);

		// Zバッファへの書き込みを有効にする。
		// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
		SetWriteZBuffer3D(TRUE);
	}
	AddFontResourceEx("data/TanueiKakuPop.otf", FR_PRIVATE, NULL);
	Game::kFontUIHandle = CreateFontToHandle("TanueiKakuPop", 40, -1);
	Game::kTitleFontHandle = CreateFontToHandle("TanueiKakuPop", 64, -1);

	//サウンドマネージャーの初期化
	soundManager_.Init();

	return true;
}

void Application::Run()
{
	SetDrawScreen(DX_SCREEN_BACK);

	//シーンの作成
	Input input;
	SceneController controller;//シーン遷移のためのコントロールオブジェクト
	//最初のシーンをセットする
	//何かしらシーンがないと,UpdateもDrawもできないため
	controller.ChangeScene(std::make_shared<TitleScene>(controller));
	while (ProcessMessage() != -1)
	{
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレーム描画を消す
		ClearDrawScreen();
		//（ゲーム内容）
		input.Update();
		UpdateEffekseer3D();
		//シーンの更新
		controller.Update(input);
		//シーンの描画
		controller.Draw();
		DrawEffekseer3D();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// 内容を画面に反映
		ScreenFlip();

		// ゲーム速度がどのPCでも同じスピードになる
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}
}

void Application::Terminate()
{
	// メモリの開放
	// shared_ptrはメモリの開放を自動で行ってくれる
	RemoveFontResourceEx("data/TanueiKakuPop.otf", FR_PRIVATE, NULL);
	Effkseer_End();
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
