#pragma once
#include <d3d11_1.h>
#include <wrl/client.h>

#include "DirectXMath.h"

class Material {
private:
	const char* name;
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	// Example of arrays holding SRVs and Sampler States for a single material
	// - The array sizes below correspond to the maximum number of SRVs and samplers that can
	// be used simultaneously (during a single draw). This is NOT the maximum number in memory.
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRVs[128];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[16];

	DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2(1, 1);
	DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2(0, 0);
public:
	Material(const char* name, DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOffset);
	~Material();

	// Get Methods
	const char* GetName() const;
	DirectX::XMFLOAT4 GetColorTint() const;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>* GetTextureSRVs();
	Microsoft::WRL::ComPtr<ID3D11SamplerState>* GetSamplers();
	DirectX::XMFLOAT2 GetUVScale() const;
	DirectX::XMFLOAT2 GetUVOffset() const;

	// Set Methods
	void SetColorTint(DirectX::XMFLOAT4 tint);
	void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> shader);
	void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> shader);
	void SetUVScale(DirectX::XMFLOAT2 scale);
	void SetUVOffset(DirectX::XMFLOAT2 offset);

	// Texture Methods
	void AddTextureSRV(unsigned int index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(unsigned int index, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState);
	void BindTexturesAndSamplers();
};