#include "CannonBall.h"
#include "Game.h"

std::vector<AudioPlayer*> CannonBall::splashes;

CannonBall::CannonBall(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState)
	: MoveableEntity(mesh, material, rasterizerState, depthStencilState)
{
	scale *= 0.2f; // changed to 0.2f, YOU'RE WELCOME!!!
	maxAccel = 20000.0f;
	maxVel = 20000.0f;
	friction = 1.0f;
	active = false;
	
	bounds = new Bounds(&position, XMFLOAT3(0.5f, 0.5f, 0.5f));
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

void CannonBall::Update(float dt, const XMMATRIX& parentMat)
{
	//active = (XMVectorGetY(position) >= -2.5f); // is cannonball is below the waterline

	if (!active)
		return;

	acceleration += XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f);

	MoveableEntity::Update(dt, parentMat);

	if (XMVectorGetY(position) <= GetYFromXZ(position, Game::vsPerFrameData->time) - 2)
	{
		active = false; 
		printf("Play splash! \n");
#if defined SOUND_PLAY
		PlaySplash();
#endif
	}

	if (bounds->Intersecting(target->GetBoundsPtr()))
	{ target->Hit(damage); active = false; }
}

void CannonBall::Fire(XMVECTOR position, XMVECTOR direction, Hittable* target, float damage)
{
	this->damage = damage;
	this->target = target;
	this->position = position;
	this->velocity = direction * 10;

	active = true;
}

float CannonBall::GetYFromXZ(XMVECTOR pos, float time)
{
	float start = (XMVectorGetX(pos) / 150.0f) * 360;
	float degsToRads = 3.1415f / 180.0f;

	float angle = (start + time * 200) * 3.1415f;
	float amplitude = 2.0f;
	float frequency = 0.25f;

	angle *= frequency;

	return sin(angle * degsToRads) * amplitude;
}

void CannonBall::PlaySplash()
{
	std::vector<AudioPlayer*> available;

	for (std::vector<AudioPlayer*>::iterator it = splashes.begin(); it != splashes.end(); it++)
	{ if (!(*it)->playing()) available.push_back((*it)); }

	if (available.size() == 0)
		return;

	int index = rand() % available.size();

	XMFLOAT3 blah;
	XMStoreFloat3(&blah, position);

	available[index]->play();
	available[index]->setPosition(blah.x, blah.y, blah.z);
}

void CannonBall::Render(ID3D11DeviceContext* deviceContext)
{
	if (active) MoveableEntity::Render(deviceContext);
}

#if defined(DEBUG) | defined(_DEBUG)
void CannonBall::DebugRender(ID3D11DeviceContext* deviceContext)
{
	if (active) MoveableEntity::DebugRender(deviceContext);
}
#endif