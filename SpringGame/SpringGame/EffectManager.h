#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Physics/Vector3.h"

class EffectManager
{
public:
	static EffectManager& GetInstance();

	void Load(const std::string& name, const char* path);
	int Play(const std::string& name, const Vector3& pos);

private:
	std::map<std::string, int>effects_;
	std::vector<int>handles_;
};

