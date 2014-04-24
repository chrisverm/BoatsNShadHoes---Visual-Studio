#include "CannonBall.h"


CannonBall::CannonBall(std::string meshId, std::string matId)
	: MoveableEntity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{
	scale *= 0.4f;
	friction = 0.9f;
	acceleration = XMVectorSet(0,-9.81f, 0,0);
}


CannonBall::~CannonBall(void)
{
}

void CannonBall::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void CannonBall::Update(float dt, const XMMATRIX& parentMat)
{
	MoveableEntity::Update(dt, parentMat);
}