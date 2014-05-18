#include "Cannon.h"

Cannon::Cannon(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState)
	: DrawableEntity(mesh, mat, rastState, depthStenState)
{ 
	scale *= 0.2f;
}

/*
Fires a cannonball!
*/
void Cannon::Fire(CannonBall* cannonBall, Hittable* target, XMVECTOR currentVeloctiy, float damage)
{
	XMMATRIX matrix = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));

	cannonBall->Fire(
		XMVector3Transform(position, matrix),
		(this->forward * (5) + this->up/2), // For some reason it looks 400x better without using current velocity...
		target,
		damage);

	cannonBall->velocity += currentVeloctiy;
}