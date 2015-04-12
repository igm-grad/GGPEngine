#pragma once
#include <unordered_map>
#include "InputManager.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class GameObject;

typedef GPPEngineAPI void(*RenderCallback)(Behavior& behavior, double deltaTime);
typedef GPPEngineAPI void(*PhysicsCallback)(Behavior& behavior, double deltaTime);
typedef GPPEngineAPI void(*KeyDownCallback)(Behavior& behavior);
typedef GPPEngineAPI void(*MouseMoveCallback)(Behavior& behavior);

class GPPEngineAPI Behavior
{
public:
	RenderCallback	renderCallback;
	PhysicsCallback physicsCallback;
	
	float	GetFloatForKey(const char* key);
	int		GetIntForKey(const char* key);

	void	SetFloatForKey(float value, const char* key);
	void	SetIntForKey(int value, const char* key);

	void			SetCallbackForKeyDown(KeyDownCallback callback, KeyCode keyCode);
	KeyDownCallback GetCallbackForKeyDown(KeyCode keyCode);
	
	GameObject*		gameObject;
	
	Behavior();
	~Behavior();

protected:
	std::unordered_map<KeyCode, KeyDownCallback>	keyInputMap;

private:
	std::unordered_map<const char*, float>			FloatMap;
	std::unordered_map<const char*, int>			IntMap;

	friend class CoreEngine;
	friend class RenderEngine;
	friend class PhysicsEngine;
};

