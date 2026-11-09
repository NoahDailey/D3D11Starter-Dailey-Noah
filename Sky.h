#pragma once

#include "Mesh.h"
#include "Camera.h"

#include <d3d11.h>
#include <wrl/client.h>

class Sky {
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	std::shared_ptr<Mesh> skyboxGeometry;
public:
	Sky(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader,
		const wchar_t* right, const wchar_t* left, const wchar_t* up, const wchar_t* down, const wchar_t* front, const wchar_t* back);
	~Sky();

	void Draw();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};