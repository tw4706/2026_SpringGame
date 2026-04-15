#include "SoundManager.h"
#include<Dxlib.h>

namespace
{
	constexpr int kBaseBgmVolume = 180;
	constexpr int kBaseSeVolume = 220;
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
	seVolume_ = kBaseSeVolume;
	bgmVolume_ = kBaseBgmVolume;

	bgmHandles_[BGM::Title] = LoadSoundMem("data/BGM・SE/Title.mp3");
	bgmHandles_[BGM::Game] = LoadSoundMem("data/BGM・SE/Game.mp3");
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

	//SEの音量
	ChangeVolumeSoundMem(seVolume_, handle);

	//SEの再生
	PlaySoundMem(handle, DX_PLAYTYPE_BACK,TRUE);
}
void SoundManager::PlayBgm(BGM bgm)
{
	int handle = bgmHandles_[bgm];

	if (currentBgmHandle_ == handle) return;

	if (currentBgmHandle_ != -1)
	{
		StopSoundMem(currentBgmHandle_);
	}

	//再生しているBGMハンドルを更新する
	currentBgmHandle_ = handle;

	//BGMの音量
	ChangeVolumeSoundMem(bgmVolume_,currentBgmHandle_);

	//BGM再生
	PlaySoundMem(currentBgmHandle_, DX_PLAYTYPE_LOOP);
}
