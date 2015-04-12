#include "GameObject.h"

Behavior* GameObject::GetBehavior()
{
	return mBehavior;
}

void GameObject::SetBehavior(Behavior* behavior)
{
	mBehavior = behavior;
}