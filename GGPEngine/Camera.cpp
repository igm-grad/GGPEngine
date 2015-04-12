#include "Camera.h"


Camera::~Camera()
{
}

void Camera::UpdateProjection(float fov, float aspectRatio, float zNear, float zFar)
{
	// Create the Projection matrix
	// This should match the window's aspect ratio, and also update anytime
	// the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fov,						// Field of View Angle
		aspectRatio,				// Aspect ratio
		zNear,						// Near clip plane distance
		zFar);						// Far clip plane distance
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P));
}

void Camera::Update()
{
	transform->getWorldTransform();
	XMVECTOR EyePosition = XMLoadFloat3(&transform->position);
	XMVECTOR UpDirection = XMLoadFloat3(&transform->up);
	XMVECTOR EyeDirection = XMLoadFloat3(&transform->forward);

	XMStoreFloat4x4(&view, XMMatrixTranspose(XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection)));
}

