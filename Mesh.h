#pragma once
#include "Vertex.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class Mesh {
private:
	// Buffers for both vertices and indices
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// ints of how many vertices and indices there are
	unsigned int verticesCount;
	unsigned int indicesCount;
	
	// Char pointer for the name
	char* name;
public:
	// Constructor/Deconstructor
	Mesh(Vertex* vertices, unsigned int verticesCount, unsigned int* indices, unsigned int indicesCount, char* name);
	~Mesh();

	// Get properties
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetVertexCount() const;
	unsigned int GetIndexCount() const;
	char* GetShapeName() const;

	// Draw method for drawing the mesh shape
	void Draw();
};