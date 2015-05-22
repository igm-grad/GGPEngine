#include "Transform.h"


Transform::Transform() : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), movementSpeed(0.001f), forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
}


Transform::~Transform()
{
}

XMMATRIX Transform::getWorldTransform()
{
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX orientation = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX proportion = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX identity = XMMatrixIdentity();

	XMVECTOR fwd = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR forwardTransform = XMVector4Transform(fwd, orientation);
	XMStoreFloat3(&forward, forwardTransform);

	XMVECTOR rg = { 1.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR rightTransform = XMVector4Transform(rg, orientation);
	XMStoreFloat3(&right, rightTransform);

	XMStoreFloat3(&up, XMVector3Cross(forwardTransform, rightTransform));

	return identity * proportion * orientation * translation;
}

void Transform::MoveForward()
{
	position.x += forward.x * movementSpeed;
	position.y += forward.y * movementSpeed;
	position.z += forward.z * movementSpeed;
}

void Transform::MoveBackward()
{
	position.x -= forward.x * movementSpeed;
	position.y -= forward.y * movementSpeed;
	position.z -= forward.z * movementSpeed;
}

void Transform::MoveLeft()
{
	position.x -= right.x * movementSpeed;
	position.y -= right.y * movementSpeed;
	position.z -= right.z * movementSpeed;
}

void Transform::MoveRight()
{
	position.x += right.x * movementSpeed;
	position.y += right.y * movementSpeed;
	position.z += right.z * movementSpeed;
}

void Transform::RotatePitch(float pitch)
{
	rotation.x += pitch * movementSpeed;

}

void Transform::RotateYaw(float yaw)
{
	rotation.y += yaw * movementSpeed;
}

void Transform::RotateRoll(float roll)
{
	rotation.z += roll * movementSpeed;
}