#include "MoveableEntity.h"

using namespace DirectX;

MoveableEntity::MoveableEntity(Mesh* mesh, Material*mat)
	: DrawableEntity(mesh,mat)
{
	velocity = XMVectorSet(0,0,0,0);
	angualrVelocity = XMVectorSet(0,0,0,0);

	acceleration = XMVectorSet(0,0,0,0);
	angularAcceleration = XMVectorSet(0,0,0,0);

	friction = 0;
}


MoveableEntity::~MoveableEntity(void)
{
}

void MoveableEntity::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	DrawableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void MoveableEntity::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	velocity += acceleration * dt;
	angualrVelocity += angularAcceleration * dt;

	velocity -= velocity * friction * dt;
	angualrVelocity -= angualrVelocity * friction * dt;
	
	position += velocity * dt;
	rotation += angualrVelocity * dt;

	DrawableEntity::Update(deviceContext, dt);
}

void MoveableEntity::Render(ID3D11DeviceContext* deviceContext)
{
	DrawableEntity::Render(deviceContext);
}