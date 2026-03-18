#pragma once
#include"Vector3.h"

class GameObject
{
public :
	GameObject(Vector3 pos,Vector3 vel);
	virtual~GameObject();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;

protected:
	Vector3 pos_;
	Vector3 vel_;
	float dir_;
};

