#include "Boat.h"

Boat::Boat(std::string meshId, std::string matId) 
	: Entity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{ }

Boat::~Boat()
{ }