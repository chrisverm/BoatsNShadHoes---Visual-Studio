#include "Water.h"

using namespace DirectX;

Water::Water(std::string meshId, std::string matId) 
	: DrawableEntity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{
	position = XMVectorSetY(position, -1);
	scale = XMVectorSet(50,1,50,0);
}


Water::~Water(void)
{
}