#pragma once
#pragma once
#include <DirectXMath.h>

using namespace DirectX;

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

class GPPEngineAPI Transform
{
public:
	XMFLOAT3	rotation;
	XMFLOAT3	position;
	XMFLOAT3	scale;

	XMFLOAT3	forward;
	XMFLOAT3	up;
	XMFLOAT3	right;

	float		movementSpeed;


	Transform();
	~Transform();

	void MoveForward();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();
	void RotatePitch(float pitch);
	void RotateYaw(float yaw);
	void RotateRoll(float roll);

	XMMATRIX getWorldTransform();

};

