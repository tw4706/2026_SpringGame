#include "GameObject.h"


GameObject::GameObject(Vector3 pos, Vector3 vel) :
	pos_(pos),
	vel_(vel),
	dir_(0.0f)
{
}

GameObject::~GameObject(){}