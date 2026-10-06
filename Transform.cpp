#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0), pitchYawRoll(0, 0, 0), scale(1, 1, 1),
	upVector(0, 1, 0), rightVector(1, 0, 0), forwardVector(0, 0, 1),
	dirtyVectors(false), dirtyMatrices(false)
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
	dirtyMatrices = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
	dirtyMatrices = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
	dirtyMatrices = true;
	dirtyVectors = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll = rotation;
	dirtyMatrices = true;
	dirtyVectors = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	dirtyMatrices = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
	dirtyMatrices = true;
}

// Getter methods
const DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

const DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

const DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

const DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	UpdateMatrices();
	return worldMatrix;
}

const DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	UpdateMatrices();
	return worldInverseTranspose;
}

const DirectX::XMFLOAT3 Transform::GetRight()
{
	UpdateVectors();
	return rightVector;
}

const DirectX::XMFLOAT3 Transform::GetUp()
{
	UpdateVectors();
	return upVector;
}

const DirectX::XMFLOAT3 Transform::GetForward()
{
	UpdateVectors();
	return forwardVector;
}

// Transformer methods
void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	dirtyMatrices = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;
	dirtyMatrices = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	// Store the input as an XMVECTOR
	XMVECTOR relInput = XMVectorSet(x, y, z, 0);

	// Create a Quaternion for the current rotation
	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Rotate the direction that we want
	XMVECTOR relOutput = XMVector3Rotate(relInput, currentRotation);

	// Store the new position
	XMStoreFloat3(&position, XMLoadFloat3(&position) + relOutput);

	dirtyMatrices = true;
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	// Store the input as an XMVECTOR
	XMVECTOR relInput = XMLoadFloat3(&offset);

	// Create a Quaternion for the current rotation
	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Rotate the direction that we want
	XMVECTOR relOutput = XMVector3Rotate(relInput, currentRotation);

	// Store the new position
	XMStoreFloat3(&position, XMLoadFloat3(&position) + relOutput);

	dirtyMatrices = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
	dirtyMatrices = true;
	dirtyVectors = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	pitchYawRoll.x += rotation.x;
	pitchYawRoll.y += rotation.y;
	pitchYawRoll.z += rotation.z;
	dirtyMatrices = true;
	dirtyVectors = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	dirtyMatrices = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	scale.x *= scale.x;
	scale.y *= scale.y;
	scale.z *= scale.z;
	dirtyMatrices = true;
}

void Transform::UpdateMatrices()
{
	if (!dirtyMatrices)
		return;

	XMMATRIX translating = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotating = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);

	// Note: Overloaded operators are defined in the DirectX namespace!
	// Alternatively, you can XMMatrixMultiply(XMMatrixMultiply(s, r), t))
	XMMATRIX world = scaling * rotating * translating;
	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(world)));

	dirtyMatrices = !dirtyMatrices;
}

void Transform::UpdateVectors()
{
	if (!dirtyVectors)
		return;

	// Get the rotation in the form of a quaternion
	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// Update the three vectors
	XMStoreFloat3(&rightVector, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), currentRotation));
	XMStoreFloat3(&upVector, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), currentRotation));
	XMStoreFloat3(&forwardVector, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), currentRotation));

	dirtyVectors = !dirtyVectors;
}
