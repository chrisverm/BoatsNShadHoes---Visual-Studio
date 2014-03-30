#include "MoveableEntity.h"

using namespace DirectX;

MoveableEntity::MoveableEntity(Mesh* mesh, Material*mat)
	: Entity(mesh,mat)
{
	velocity = XMVectorSet(0,0,0,0);
	angualrVelocity = XMVectorSet(0,0,0,0);
}


MoveableEntity::~MoveableEntity(void)
{
}

void MoveableEntity::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	Entity::Initialize(modelConstBuffer, modelConstBufferData);
}

void MoveableEntity::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	Entity::Update(deviceContext, dt);

	position += velocity * dt;

	// do nothing with angular for right now.
}

void MoveableEntity::Render(ID3D11DeviceContext* deviceContext)
{
	Entity::Render(deviceContext);
}