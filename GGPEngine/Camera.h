#pragma once
#include <DirectXMath.h>
#include "Transform.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class  Camera
{
public:
	
	Transform*	transform;
	
	XMFLOAT4X4	view;
	XMFLOAT4X4	projection;


	Camera() {};
	~Camera();


	void UpdateProjection(float fov, float aspectRatio, float zNear, float zFar);
	void Update();

};

