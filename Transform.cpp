#include "Transform.h"

using namespace DirectX;

Transform::Transform():
	position(0,0,0), pitchYawRoll(0,0,0), scale(1,1,1)
{
	// Initialize the matrices
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

Transform::~Transform()
{
}

// Setter methods
void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	UpdateMatrices();
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;

	UpdateMatrices();
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;

	UpdateMatrices();
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll = rotation;

	UpdateMatrices();
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	UpdateMatrices();
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;

	UpdateMatrices();
}

// Getter methods
DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTranspose;
}

// Transformer methods
void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	UpdateMatrices();
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;

	UpdateMatrices();
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;

	UpdateMatrices();
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll.x += rotation.x;
	pitchYawRoll.y += rotation.y;
	pitchYawRoll.z += rotation.z;

	UpdateMatrices();
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;

	UpdateMatrices();
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	scale.x *= scale.x;
	scale.y *= scale.y;
	scale.z *= scale.z;

	UpdateMatrices();
}

void Transform::UpdateMatrices()
{
	XMMATRIX translating = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotating = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);

	// Note: Overloaded operators are defined in the DirectX namespace!
	// Alternatively, you can XMMatrixMultiply(XMMatrixMultiply(s, r), t))
	XMMATRIX world = scaling * rotating * translating;
	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(world)));
}
