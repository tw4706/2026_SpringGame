#include "EffectManager.h"
#include"EffekseerForDXLib.h"
#include<cassert>
#include "../Application.h"

EffectManager& EffectManager::GetInstance()
{
	static EffectManager instance;
	return instance;
}

void EffectManager::Update()
{
	for (auto it = handles_.begin(); it != handles_.end();)
	{
		if (IsEffekseer3DEffectPlaying(*it) == FALSE)
		{
			it = handles_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void EffectManager::Load(const std::string& name, const char* path)
{
	int handle = LoadEffekseerEffect(path);
	effects_[name] = handle;
}

int EffectManager::Play(const std::string& name, const Vector3& pos)
{
	assert(effects_.count(name) > 0);

	int handle = PlayEffekseer3DEffect(effects_[name]);

	SetPosPlayingEffekseer3DEffect(
		handle,
		pos.x_, pos.y_, pos.z_);

	handles_.push_back(handle);

	return handle;
}
