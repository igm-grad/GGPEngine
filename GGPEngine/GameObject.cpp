#include "GameObject.h"

XMMATRIX GameObject::getWorldTransform()
{
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX orientation = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
	XMMATRIX proportion = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX identity = XMMatrixIdentity();
	return identity * proportion * orientation * translation;
}

void GameObject::translateForward(float distance)
{
	XMMATRIX transform = getWorldTransform();
	XMVECTOR forward = { 0.0f, 0.0f, 1.0f, 0.0f};
	XMVECTOR forwardTransform = XMVector4Transform(forward, transform);
	
	XMFLOAT4 newPosition;
	XMStoreFloat4(&newPosition, forwardTransform);
	position.x += distance * newPosition.x;
	position.y += distance * newPosition.y;
	position.z += distance * newPosition.z;
}

void GameObject::translateRight(float distance)
{

}

void GameObject::translateUp(float distance)
{

}

void GameObject::applyRotation(XMFLOAT3X3 rotation)
{

}
void GameObject::rotateHeading(float angle)
{

}

void GameObject::rotatePitch(float angle)
{

}

void GameObject::rotateYaw(float angle)
{

}