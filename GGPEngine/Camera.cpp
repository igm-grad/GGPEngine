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
}

void Camera::Update()
{
	XMVECTOR EyePosition = XMLoadFloat3(&position);
	XMVECTOR UpDirection = XMLoadFloat3(&up);
	XMVECTOR EyeDirection = XMLoadFloat3(&forward);
	XMVECTOR rollPitchYaw = XMLoadFloat3(&rotation);
	XMVECTOR orientation = XMQuaternionRotationRollPitchYawFromVector(rollPitchYaw);
	EyeDirection = XMQuaternionMultiply(EyeDirection, orientation);
	XMStoreFloat3(&forward,EyeDirection);
	XMStoreFloat4x4(&view, XMMatrixTranspose(XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection)));
	rotation = { 0.0f, 0.0f, 0.0f };
}

void Camera::MoveForward()
{
	position.x += forward.x * movementSpeed;
	position.y += forward.y * movementSpeed;
	position.z += forward.z * movementSpeed;
}

void Camera::MoveBackward()
{
	position.x -= forward.x * movementSpeed;
	position.y -= forward.y * movementSpeed;
	position.z -= forward.z * movementSpeed;
}

void Camera::MoveRight()
{
	XMVECTOR V1 = XMLoadFloat3(&forward);
	XMVECTOR V2 = XMLoadFloat3(&up);
	
	XMFLOAT3 right;
	XMStoreFloat3(&right, XMVector3Cross(V1, V2));

	position.x -= right.x * movementSpeed;
	position.y -= right.y * movementSpeed;
	position.z -= right.z * movementSpeed;
}

void Camera::MoveLeft()
{
	XMVECTOR V1 = XMLoadFloat3(&forward);
	XMVECTOR V2 = XMLoadFloat3(&up);

	XMFLOAT3 right;
	XMStoreFloat3(&right, XMVector3Cross(V1, V2));

	position.x += right.x * movementSpeed;
	position.y += right.y * movementSpeed;
	position.z += right.z * movementSpeed;
}

void Camera::RotatePitch(float pitch)
{
	rotation.x += pitch * movementSpeed;

}

void Camera::RotateYaw(float yaw)
{
	rotation.y += yaw * movementSpeed;
}