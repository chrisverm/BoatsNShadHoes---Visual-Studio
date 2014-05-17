#include "Cannon.h"

Cannon::Cannon(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState)
	: DrawableEntity(mesh, mat, rastState, depthStenState)
{ 
	scale *= 0.2f;
}

void Cannon::Fire(CannonBall* cannonBall, Hittable* target, XMVECTOR currentVeloctiy, float damage)
{
	XMMATRIX matrix = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));

	cannonBall->Fire(XMVector3Transform(position, matrix),(this->forward + this->up), target, damage);
	cannonBall->velocity += currentVeloctiy;
}