#pragma once
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"

#ifdef _WINDLL
#define GPPEngineAPI   __declspec( dllexport )
#else
#define GPPEngineAPI   __declspec( dllimport )
#endif

using namespace DirectX;

class  GameObject
{
public:

	XMFLOAT4	rotation;
	XMFLOAT3	position;
	XMFLOAT3	scale;
	Mesh*		mesh;
	Material*	material;

	GameObject(Mesh* mesh) : mesh(mesh), position({ 0.0f, 0.0f, 0.0f }), rotation({ 0.0f, 0.0f, 0.0f, 1.0f }), scale({ 1.0f, 1.0f, 1.0f }) {};
	GameObject() : GameObject(NULL) {};
	~GameObject() { delete(mesh); delete(material); };

	XMMATRIX getWorldTransform();
	void translateForward(float distance);
	void translateRight(float distance);
	void translateUp(float distance);

	void applyRotation(XMFLOAT3X3 rotation);
	void rotateHeading(float angle);
	void rotatePitch(float angle);
	void rotateYaw(float angle);
};

