#pragma once
#include "Mesh.h"
#include "GameEntities.h"
#include "Camera.h"
#include "Material.h"

#include <d3d11.h>
#include <d3d11shadertracing.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();
	void ImGuiCreate(float deltaTime);
	Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(const wchar_t* filePath);
	void FillAndBindNextConstantBuffer(void* data, unsigned int dataSizeInBytes, D3D11_SHADER_TYPE_DESC shaderType, unsigned int registerSlot);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderUV;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderNormal;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderCustom;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> PSConstantBuffer;

	// ImGui-related variables
	DirectX::XMFLOAT4 backgroundColor;
	bool demoVisible;
	int menuStyle;
	std::vector<DirectX::XMFLOAT4> colorTints;
	DirectX::XMFLOAT3 offset;

	// Create shared_ptr vector for the mesh shapes
	std::vector<std::shared_ptr<Mesh>> meshes;

	std::vector<std::shared_ptr<GameEntities>> gameObjects;

	// Create a Camera object
	std::vector<std::shared_ptr<Camera>> cameras;
	int activeCamera;

	// Create a shared_ptr vector for Materials
	std::vector<std::shared_ptr<Material>> materials;

	// The D3D11.1 version of the Context object
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context1;
	// The one and only very large constant buffer (our GPU memory "heap")
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBufferHeap;
	// Size of the constant buffer heap (measured in bytes)
	unsigned int cbHeapSizeInBytes;
	// Position of the next unused portion of the heap
	unsigned int cbHeapOffsetInBytes;
};