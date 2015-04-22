#include "Camera.h"


Camera::~Camera()
{
}

void Camera::UpdateProjection(float fov, float aspectRatio, float zNear, float zFar)
{
	this->fov = fov;
	this->farPlane = zFar;

	// Create the Projection matrix
	// This should match the window's aspect ratio, and also update anytime
	// the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fov,						// Field of View Angle
		aspectRatio,				// Aspect ratio
		zNear,						// Near clip plane distance
		zFar);						// Far clip plane distance
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P));

	cubemap = nullptr;
}

void Camera::Update()
{
	transform->getWorldTransform();
	XMVECTOR EyePosition = XMLoadFloat3(&transform->position);
	XMVECTOR UpDirection = XMLoadFloat3(&transform->up);
	XMVECTOR EyeDirection = XMLoadFloat3(&transform->forward);

	XMStoreFloat4x4(&view, XMMatrixTranspose(XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection)));

	if (cubemap != nullptr)
	{
		cubemap->update(*transform);

		cubemap->draw(view, projection);
	}
}

void Camera::createCubeMap(ID3D11Device* device, ID3D11DeviceContext* DeviceContext)
{
	cubemap = new SkyBox();

	cubemap->init(device, DeviceContext);
}