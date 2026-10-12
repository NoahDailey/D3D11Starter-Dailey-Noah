#include "GameEntities.h"

GameEntities::GameEntities(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	transform = std::make_shared<Transform>();
	this->mesh = mesh;
	this->material = material;
}

GameEntities::~GameEntities()
{
}

std::shared_ptr<Transform> GameEntities::GetTransform()
{
	return transform;
}

std::shared_ptr<Mesh> GameEntities::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntities::GetMaterial()
{
	return material;
}

void GameEntities::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

void GameEntities::Draw()
{
	mesh->Draw();
}
