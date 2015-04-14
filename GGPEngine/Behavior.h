#pragma once
#pragma warning( disable: 4251 )
#include <unordered_map>
#include <map>
#include "InputManager.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class GameObject;

typedef GPPEngineAPI void(*RenderCallback)(GameObject& gameObject, double deltaTime);
typedef GPPEngineAPI void(*PhysicsCallback)(GameObject& gameObject, double deltaTime);
typedef GPPEngineAPI void(*KeyCallback)(GameObject& gameObject);
typedef GPPEngineAPI void(*KeyDownCallback)(GameObject& gameObject);
typedef GPPEngineAPI void(*MouseMoveCallback)(GameObject& gameObject);

class GPPEngineAPI Behavior
{
public:
	RenderCallback	renderCallback;
	PhysicsCallback physicsCallback;
	
	float	GetFloatForKey(const char* key);
	int		GetIntForKey(const char* key);

	void	SetFloatForKey(float value, const char* key);
	void	SetIntForKey(int value, const char* key);

	void	SetCallbackForKeyDown(KeyDownCallback callback, KeyCode keyCode);
	void	SetCallbackForKey(KeyCallback callback, KeyCode keyCode);

	Behavior();
	~Behavior();

protected:
	std::map<KeyCode, KeyCallback>	keyInputMap;
	std::map<KeyCode, KeyDownCallback>	keyDownInputMap;

private:
	std::unordered_map<const char*, float>			FloatMap;
	std::unordered_map<const char*, int>			IntMap;

	friend class CoreEngine;
	friend class RenderEngine;
	friend class PhysicsEngine;
};

