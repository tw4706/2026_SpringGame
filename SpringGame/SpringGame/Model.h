#pragma once
#include"../Physics/Vector3.h"
#include<string>
#include<Dxlib.h>
#include <unordered_map>

class Model
{
public:
	Model();

	void Load(const std::string&path);

	void Draw();

	void SetPosition(const Vector3& pos);

	void SetScale(const Vector3& scale);

	void SetRotationY(float angle);

	int GetHandle()const { return handle_; }

	void Release();

private:
	int handle_;

	static std::unordered_map<std::string, int> sModelMap_;
};

