#pragma once
#include"../Physics/Vector3.h"
#include<string>
#include<Dxlib.h>
#include <unordered_map>

class Model
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Model();

	/// <summary>
	/// モデルのロード
	/// </summary>
	/// <param name="path">モデルのパス</param>
	void Load(const std::string&path);

	/// <summary>
	/// モデルの描画
	/// </summary>
	void Draw();

	/// <summary>
	/// モデルのポジションのセット
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// モデルのスケール
	/// </summary>
	/// <param name="scale">モデルの拡大率</param>
	void SetScale(const Vector3& scale);

	/// <summary>
	/// モデルのY軸回転
	/// </summary>
	/// <param name="angle">角度</param>
	void SetRotationY(float angle);

	/// <summary>
	/// モデルハンドルの取得
	/// </summary>
	/// <returns>ハンドルを返す</returns>
	int GetHandle()const { return handle_; }

	/// <summary>
	/// モデルの解放
	/// </summary>
	void Release();

private:
	//ハンドル
	int handle_;

	//モデルのパス
	static std::unordered_map<std::string, int> sModelMap_;
};

