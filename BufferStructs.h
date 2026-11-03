#pragma once
#include "DirectXMath.h"
#include "Lights.h"

#include <vector>

/// <summary>
/// A struct that contains data that will be sent and kept
/// on the GPU for the Vertex Shader
/// </summary>
struct VSExternalData
{
	DirectX::XMFLOAT4X4		World;				// A change in transform
	DirectX::XMFLOAT4X4		ViewMatrix;			// A camera view matrix
	DirectX::XMFLOAT4X4		ProjectionMatrix;	// A camera projection matrix
	DirectX::XMFLOAT4X4		WorldInvTranspose;	// An inverse of the world transform
};

/// <summary>
/// A struct that contains data that will be sent and kept
/// on the GPU for the Pixel Shader
/// </summary>
struct PSExternalData
{
	Light					Lights[MAX_NUMBER_LIGHTS];			// A test directional Light

	float					LightsCount;
	DirectX::XMFLOAT3		AmbientColor;		// Ambient color of the pixel

	DirectX::XMFLOAT3		CameraPosition;		// Current position of the camera
	float					padding;

	DirectX::XMFLOAT3		ColorTint;			// A tint color of a pixel
	float					morePadding;

	DirectX::XMFLOAT2		UVScale;
	DirectX::XMFLOAT2		UVOffset;
};