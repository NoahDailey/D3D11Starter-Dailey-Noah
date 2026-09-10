#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class Mesh 
{
public:
	// Construction and Deconstruction
	Mesh(Vertex vertices[], unsigned int indices[]);
	~Mesh();
	
	// Methods for creating the buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	// Methods for recieving the count of the buffers
	int GetIndexCount();
	int GetVertexCount();

	// Method for drawing to the screen
	void Draw();

private:
	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Ints to track the number of indices and vertices
	int totalIndices = 0;
	int totalVertices = 0;
};