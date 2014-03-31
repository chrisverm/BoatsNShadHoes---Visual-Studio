#include "Boat.h"

using namespace DirectX;
Boat::Boat(std::string meshId, std::string matId) 
	: MoveableEntity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{ 
	angualrVelocity = XMVectorSet(0,1,1,0);
	//velocity = XMVectorSet(0,0.25f,0.5f,0);

	//friction = 0.9;
}

Boat::~Boat()
{ }

void Boat::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void Boat::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	MoveableEntity::Update(deviceContext, dt);
}