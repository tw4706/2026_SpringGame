#pragma once

// ゲーム全体で使用する定数
namespace Game
{
	// 画面情報
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
	constexpr int kColorBitNum = 32;

	//externは変数の宣言だけ行い、実体は別のcppで定義される
	//UI用フォントのハンドル
	extern int kFontUIHandle;
	extern int kTitleFontHandle;
}
