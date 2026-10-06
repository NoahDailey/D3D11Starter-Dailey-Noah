#pragma once

#include "Input.h"
#include "Transform.h"

#include "DirectXMath.h"

class Camera {
private:
	// Transform for movement
	Transform transform;

	// View and Projection Matrices for camera range
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Customization fields
	float fovAngle;
	float nearClipPlane;
	float farClipPlane;
	float movementSpeed;
	float mouseLookSpeed;
public:
	// Construction and Deconstruction
	Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, DirectX::XMFLOAT3 startingOrientation,
		float fov, float nearClipPlane, float farClipPlane, float movementSpeed, float mouseLookSpeed);
	~Camera();

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	// Update methods
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float deltaTime);
};