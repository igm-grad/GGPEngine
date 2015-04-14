#pragma once
#pragma warning( disable: 4251 )

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class  PhysicsEngine
{
public:
	double currentTime;

	PhysicsEngine();
	~PhysicsEngine();

	bool Initialize();
	void Update(float totalTime);
};

