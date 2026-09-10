#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class Mesh 
{
public:
	/// <summary>
	/// Get access to the vertex buffer
	/// </summary>
	/// <returns>A Comptr to the vertex buffer</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	/// <summary>
	/// Get access to the index buffer
	/// </summary>
	/// <returns>A ComPtr to the Index Buffer</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

private:
	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Ints to track the number of indeces and vertices
	int totalIndeces;
	int totalVertices;
};