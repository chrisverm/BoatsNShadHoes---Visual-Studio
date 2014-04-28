#include "CannonBall.h"


CannonBall::CannonBall(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState)
	: MoveableEntity(mesh, material, rasterizerState)
{
	scale *= 10.0f; // change to 0.4f please :D
	maxAccel = 20000.0f;
	maxVel = 20000.0f;
	friction = 1.0f;
}

CannonBall::~CannonBall(void)
{
}

void CannonBall::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}
#include <iostream>
void CannonBall::Update(float dt, const XMMATRIX& parentMat)
{
	inPlay = (XMVectorGetY(position) >= -2.5f); // is cannonball is below the waterline

	if (!inPlay)
		return;

	acceleration += XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f);

	MoveableEntity::Update(dt, parentMat);
}

void CannonBall::Fire(XMVECTOR position, XMVECTOR direction)
{
	this->position = position;
	this->velocity = direction * 10;
}