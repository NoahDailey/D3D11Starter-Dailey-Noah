#include "GameEntities.h"

GameEntities::GameEntities(std::shared_ptr<Mesh> mesh)
{
	transform = std::make_shared<Transform>();
	this->mesh = mesh;
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

void GameEntities::Draw()
{
	mesh->Draw();
}
