#include "MoveableEntity.h"

using namespace DirectX;

MoveableEntity::MoveableEntity(Mesh* mesh, Material*mat)
	: DrawableEntity(mesh,mat)
{
	velocity = XMVectorSet(0,0,0,0);
	angularVelocity = XMVectorSet(0,0,0,0);
	maxVel = 0;

	acceleration = XMVectorSet(0,0,0,0);
	angularAcceleration = XMVectorSet(0,0,0,0);
	maxAccel = 0;

	friction = 0;
}


MoveableEntity::~MoveableEntity(void)
{
}

void MoveableEntity::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	DrawableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void MoveableEntity::Update(float dt, const XMMATRIX& parentMat)
{
	XMVector3ClampLength(acceleration, -maxAccel, maxAccel);

	velocity += acceleration * dt;
	angularVelocity += angularAcceleration * dt;

	XMVector3ClampLength(velocity, -maxVel, maxVel);

	velocity -= velocity * friction * dt;
	angularVelocity -= angularVelocity * friction * dt;
	
	position += velocity * dt;
	rotation += angularVelocity * dt;

	DrawableEntity::Update(dt, parentMat);
}

void MoveableEntity::Render(ID3D11DeviceContext* deviceContext)
{
	DrawableEntity::Render(deviceContext);
}