#include "Behavior.h"
#include <stdio.h>

void EmptyCallBack(GameObject& gameObject, double deltaTime) {}

Behavior::Behavior()
{
	renderCallback = EmptyCallBack;
	physicsCallback = EmptyCallBack;
}

Behavior::~Behavior()
{

}

float Behavior::GetFloatForKey(const char* key)
{
	return FloatMap.at(key);
}

int Behavior::GetIntForKey(const char* key)
{
	return IntMap.at(key);
}

void Behavior::SetFloatForKey(float value, const char* key)
{
	FloatMap[key] = value;
}

void Behavior::SetIntForKey(int value, const char* key)
{
	IntMap[key] = value;
}

void Behavior::SetCallbackForKey(KeyCallback callback, KeyCode keyCode)
{
	keyInputMap[keyCode] = callback;

}

void Behavior::SetCallbackForKeyDown(KeyDownCallback callback, KeyCode keyCode)
{
	keyDownInputMap[keyCode] = callback;
}

