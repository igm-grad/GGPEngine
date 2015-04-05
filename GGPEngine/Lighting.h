#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	XMFLOAT4	ambientColor;
	XMFLOAT4	diffuseColor;
	XMFLOAT3	direction;
	float		padding;
};

struct PointLight
{
	XMFLOAT4	ambientColor;
	XMFLOAT4	diffuseColor;
	XMFLOAT3	position;
	float		radius;
};

struct SpotLight
{
	XMFLOAT4	ambientColor;
	XMFLOAT4	diffuseColor;
	XMFLOAT3	direction;
	XMFLOAT3	position;
	XMFLOAT2	padding;
};