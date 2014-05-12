#include "CannonBall.h"


CannonBall::CannonBall(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState)
	: MoveableEntity(mesh, material, rasterizerState, depthStencilState)
{
	scale *= 0.2f; // changed to 0.2f, YOU'RE WELCOME!!!
	maxAccel = 20000.0f;
	maxVel = 20000.0f;
	friction = 1.0f;
	active = false;
}

CannonBall::~CannonBall(void)
{
}

void CannonBall::SetVelocity(XMVECTOR value)
{
	// stuff happens here! :)
}

/*
 * Returns whether or not the CannonBall is active 
 *
 * @return	If the cannonball is currently active
 */
bool CannonBall::Active() const { return active; }

void CannonBall::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

#include <iostream>
void CannonBall::Update(float dt, const XMMATRIX& parentMat)
{
	//active = (XMVectorGetY(position) >= -2.5f); // is cannonball is below the waterline

	if (!active)
		return;

	acceleration += XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f);

	if(XMVectorGetY(position) <= -2.5f)
		active = false;

	MoveableEntity::Update(dt, parentMat);
}

void CannonBall::Fire(XMVECTOR position, XMVECTOR direction)
{
	this->position = position;
	this->velocity = direction * 10;

	active = true;
}