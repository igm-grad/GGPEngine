#pragma once
#include <DirectXMath.h>

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class  Camera
{
public:
	
	XMFLOAT3	position;
	XMFLOAT3	forward;
	XMFLOAT3	up;
	XMFLOAT3	rotation;
	XMFLOAT4X4	view;
	XMFLOAT4X4	projection;
	float		movementSpeed;
	float		fov;
	float		farPlane;

	Camera() : position({ 0.0f, 0.0f, 0.0f }), rotation({ 0.0f, 0.0f, 0.0f }), forward({ 0.0f, 0.0f, 1.0f }), up({0.0f, 1.0f, 0.0f}), movementSpeed(0.01f) {};
	~Camera();


	void UpdateProjection(float fov, float aspectRatio, float zNear, float zFar);
	void Update();
	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();
	void RotatePitch(float pitch);
	void RotateYaw(float yaw);
};

