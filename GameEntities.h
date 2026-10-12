#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <memory>

class GameEntities {
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
public:
	// Construction Deconstruction
	GameEntities(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	~GameEntities();

	// Getters
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();

	// Setters
	void SetMaterial(std::shared_ptr<Material> material);

	// Other methods
	void Draw();
};