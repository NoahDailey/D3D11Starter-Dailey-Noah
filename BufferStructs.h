#pragma once
#include "DirectXMath.h"

/// <summary>
/// A struct that contains data that will be sent and kept on the GPU
/// </summary>
struct VSExternalData
{
	DirectX::XMFLOAT4		TintColor;  // A change of the color
	DirectX::XMFLOAT3		Offset;		// A change in position
};