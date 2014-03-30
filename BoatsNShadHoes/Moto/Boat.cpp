#include "Boat.h"

using namespace DirectX;
Boat::Boat(std::string meshId, std::string matId) 
	: MoveableEntity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{ }

Boat::~Boat()
{ }

void Boat::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);

	//velocity = XMVectorSet(0.1f, 0.0f, 0.0f, 0.0f);
}