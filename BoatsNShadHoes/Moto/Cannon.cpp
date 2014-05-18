#include "Cannon.h"

Cannon::Cannon(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState)
	: DrawableEntity(mesh, mat, rastState, depthStenState)
{ 
	scale *= 0.2f;
	coolDownTime = 2;
}

/*
Sets the cool down time and red tint for the cannon.
*/
void Cannon::SetStats(CannonStats stats)
{
	coolDownTime = stats.coolDownTime;
	redTint = stats.redTint;
}

/*
Update, calculates tint and sets active to true once cooldown is over.
*/
void Cannon::Update(float dt, const XMMATRIX& parentMat)
{
	if (coolDown > 0)
	{
		coolDown-= dt;
		tint.x = (coolDown/coolDownTime) * redTint;
	}
	else if (!active)
	{
		tint.x = 0;
		active = true;
	}

	DrawableEntity::Update(dt, parentMat);
}

/*
Fires a cannonball if not cooling down.
*/
void Cannon::Fire(CannonBall* cannonBall, Hittable* target, XMVECTOR currentVeloctiy, float damage)
{
	if (active)
	{
		// Set tint and inactive.
		tint.x = redTint;
		coolDown = coolDownTime;
		active = false;

		XMMATRIX matrix = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));

		cannonBall->Fire(
			XMVector3Transform(position, matrix),
			(this->forward * (5) + this->up/2), // For some reason it looks 400x better without using current velocity...
			target,
			damage);

		cannonBall->velocity += currentVeloctiy;
	}
}