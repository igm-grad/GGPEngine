#pragma once
#pragma warning( disable: 4251 )
#include <DirectXMath.h>
#include "GameObject.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class  Terrain : public GameObject
{

public:


	int width; // Size of the terrain in the x-axis
	int depth; // Size of the terrain in the y-axis
	int height; // size of the terrain in the z-axis


	// Constructor and Destructor
	Terrain(Mesh* mesh) : mesh(mesh) {};
	Terrain() : Terrain(NULL) {};
	~Terrain() { delete(mesh); delete(material); delete(transform);  delete(behavior); };

	// Member functions

	Mesh createPlane()

};

