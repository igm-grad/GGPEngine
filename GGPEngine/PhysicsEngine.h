#pragma once

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class GPPEngineAPI PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();
};

