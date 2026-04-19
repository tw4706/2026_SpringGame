#pragma once
#include"../Manager/SoundManager.h"
class Application
{
private:
	Application();
	Application(const Application& app) = delete;
	void operator=(const Application& app) = delete;

	SoundManager soundManager_;
public:
	~Application();

	/// <summary>
	/// Applicationのインスタンスを取得する
	/// </summary>
	/// <returns>Applicationの実態はこの関数内部で作成する</returns>
	static Application& GetInstance();

	bool Init();

	void Run();
	void Terminate();

	/// <summary>
	/// サウンドマネージャーを取得する
	/// </summary>
	/// <returns>サウンドマネージャーの参照</returns>
	SoundManager& GetSoundManager() { return soundManager_; }
};

