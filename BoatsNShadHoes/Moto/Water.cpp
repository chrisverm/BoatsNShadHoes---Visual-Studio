#include "Water.h"

using namespace DirectX;

Water::Water(std::string meshId, std::string matId) 
	: Entity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{
	position = XMVectorSetY(position, -1);
	scale = XMVectorSet(100,1,100,0);
}


Water::~Water(void)
{
}
