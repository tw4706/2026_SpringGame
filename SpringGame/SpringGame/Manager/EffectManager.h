#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Physics/Vector3.h"

class EffectManager
{
public:
	static EffectManager& GetInstance();

	void Update();

	/// <summary>
	/// エフェクトのロード
	/// </summary>
	/// <param name="name">エフェクトの文字列</param>
	/// <param name="path">エフェクトのパス</param>
	void Load(const std::string& name, const char* path);

	/// <summary>
	/// エフェクトの再生
	/// </summary>
	/// <param name="name">エフェクトの名前</param>
	/// <param name="pos">エフェクトを生成する座標</param>
	/// <returns>ハンドルを返す</returns>
	int Play(const std::string& name, const Vector3& pos);

	/// <summary>
	/// エフェクトの停止
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void Stop(int handle);

private:
	//エフェクトハンドルを管理するマップ
	std::map<std::string, int>effects_;

	//エフェクトハンドルを管理する配列
	std::vector<int>handles_;
};

