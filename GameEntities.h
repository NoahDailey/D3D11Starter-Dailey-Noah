#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>

class GameEntities {
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
public:
	// Construction Deconstruction
	GameEntities(std::shared_ptr<Mesh> mesh);
	~GameEntities();

	// Getters
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Mesh> GetMesh();

	// Other methods
	void Draw();
};