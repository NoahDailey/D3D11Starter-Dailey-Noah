#pragma once

#include "DirectXMath.h"

class Transform {
private:
	// Basic Vector Data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;

	// Relative direction vectors
	DirectX::XMFLOAT3 upVector;
	DirectX::XMFLOAT3 rightVector;
	DirectX::XMFLOAT3 forwardVector;

	// Matrix Data
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTranspose;

	// Booleans for tracking changes in matrices and direction vectors
	bool dirtyVectors;
	bool dirtyMatrices;
public:
	// Constrcutor & Deconstructor
	Transform();
	~Transform();

	// Setter methods
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation); // XMFLOAT4 for quarternions
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	// Getter methods
	const DirectX::XMFLOAT3 GetPosition();
	const DirectX::XMFLOAT3 GetPitchYawRoll(); // XMFLOAT4 for quarternions
	const DirectX::XMFLOAT3 GetScale();
	const DirectX::XMFLOAT4X4 GetWorldMatrix();
	const DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	const DirectX::XMFLOAT3 GetRight();
	const DirectX::XMFLOAT3 GetUp();
	const DirectX::XMFLOAT3 GetForward();

	// Transformer methods
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void MoveRelative(DirectX::XMFLOAT3 offset);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	// Update methods
	void UpdateMatrices();
	void UpdateVectors();
};