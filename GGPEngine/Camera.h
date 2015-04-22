#pragma once
#pragma warning( disable: 4251 )

#include <d3d11.h>
#include "Transform.h"
#include <vector>
#include "SkyBox.h"

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
	float		fov;
	float		farPlane;

	SkyBox* cubemap;

	Camera() {};
	~Camera();


	void UpdateProjection(float fov, float aspectRatio, float zNear, float zFar);
	void Update();
	void createCubeMap(ID3D11Device* device, ID3D11DeviceContext* DeviceContext);
};

