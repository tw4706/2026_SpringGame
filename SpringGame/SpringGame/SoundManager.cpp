#include "SoundManager.h"
#include<Dxlib.h>

namespace
{
	constexpr int kBaseVolume = 80;
}

SoundManager::SoundManager():
	currentSeHandle_(-1),
	currentBgmHandle_(-1)
{
}

SoundManager::~SoundManager()
{
	//BGMハンドルの削除
	for (auto bgm : bgmHandles_)
	{
		DeleteSoundMem(bgm.second);
	}

	//SEハンドルの削除
	for (auto se : seHandles_)
	{
		DeleteSoundMem(se.second);
	}
}

void SoundManager::Init()
{
	volume_ = kBaseVolume;

	bgmHandles_[BGM::Title] = LoadSoundMem("data/BGM・SE");
	bgmHandles_[BGM::Game] = LoadSoundMem("data/BGM・SE");
	bgmHandles_[BGM::Result] = LoadSoundMem("data/BGM・SE");

	seHandles_[SE::Decide] = LoadSoundMem("data/BGM・SE/decide.mp3");
	seHandles_[SE::Attack] = LoadSoundMem("data/BGM・SE/attack.mp3");
	seHandles_[SE::Move] = LoadSoundMem("data/BGM・SE/move.mp3");
	seHandles_[SE::Hit] = LoadSoundMem("data/BGM・SE/enemyHit.mp3");
	seHandles_[SE::Dodge] = LoadSoundMem("data/BGM・SE/dodge.mp3");
	seHandles_[SE::JustDodge] = LoadSoundMem("data/BGM・SE/justDodge.wav");
	seHandles_[SE::PlayerDeath] = LoadSoundMem("data/BGM・SE/playerDeath.mp3");
}

void SoundManager::PlaySe(SE se)
{
	int handle = seHandles_[se];
	if (currentSeHandle_ == handle)return;

	//再生中のSEがある場合は停止
	if (currentSeHandle_ != -1)
	{
		StopSoundMem(currentSeHandle_);
	}

	//再生中のSEハンドルを更新
	currentSeHandle_ = handle;

	//SEの音量
	ChangeVolumeSoundMem(255, currentSeHandle_);

	//SEの再生
	PlaySoundMem(handle, DX_PLAYTYPE_BACK);
}
void SoundManager::PlayBgm(BGM bgm)
{
	int handle = bgmHandles_[bgm];

	//再生しているBGMハンドルを更新
	currentBgmHandle_ = handle;

	//BGMの音量
	ChangeVolumeSoundMem(180,currentBgmHandle_);

	//BGM再生
	PlaySoundMem(currentBgmHandle_, DX_PLAYTYPE_LOOP);
}
