#pragma once
#include <d3d11_1.h>
#include <wrl/client.h>

#include "DirectXMath.h"

class Material {
private:
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
public:
	Material(DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);
	~Material();

	// Get Methods
	DirectX::XMFLOAT4 GetColorTint();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();

	// Set Methods
	void SetColorTint(DirectX::XMFLOAT4 tint);
	void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader);
	void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader);
};