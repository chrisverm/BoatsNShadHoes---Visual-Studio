#include "CannonBall.h"
#include "Game.h"

std::vector<AudioPlayer*> CannonBall::splashes;

#pragma region Constructor

/*
Constructor for a cannonball. 
Calls base Moveable Entity.
Also sets things like maxVel/Accel, friction, bounding box, etc.
*/
CannonBall::CannonBall(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState)
	: MoveableEntity(mesh, material, rasterizerState, depthStencilState)
{
	scale *= 0.2f; // changed to 0.2f, YOU'RE WELCOME!!!
	maxAccel = 20000.0f;
	maxVel = 20000.0f;
	friction = 1.0f;
	active = false;
	
	acceleration = XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f);

	bounds = new Bounds(&position, XMFLOAT3(0.5f, 0.5f, 0.5f));
}

#pragma endregion

#pragma region Update,Render

/*
Update.
Calls base Moveable Entity Update.
If inactive does nothing.
Handles collision with target and water.
*/
void CannonBall::Update(float dt, const XMMATRIX& parentMat)
{
	if (!active)
		return;
	
	MoveableEntity::Update(dt, parentMat);

	if (XMVectorGetY(position) <= GetYFromXZ(position, Game::vsPerFrameData->time) - 2)
	{
		active = false; 
#if defined SOUND_PLAY
		PlaySplash();
#endif
	}

	if (bounds->Intersecting(target->GetBoundsPtr()))
	{ target->Hit(damage); active = false; }
}

/*
If not active, dosnt call moveable entity render.
*/
void CannonBall::Render(ID3D11DeviceContext* deviceContext)
{
	if (active) MoveableEntity::Render(deviceContext);
}

#if defined(DEBUG) | defined(_DEBUG)
/*
If not active, dosnt call moveable entity debugrender.
*/
void CannonBall::DebugRender(ID3D11DeviceContext* deviceContext)
{
	if (active) MoveableEntity::DebugRender(deviceContext);
}
#endif

#pragma endregion

#pragma region Getters

/*
Returns whether or not the CannonBall is active 
*/
bool CannonBall::Active() const { return active; }

#pragma endregion

#pragma region Fire

/*
Fires this cannonball (sets it to active).
Requires a position to fire from, direction to travel in, a damage value, and the target to check collisions with.
*/
void CannonBall::Fire(XMVECTOR position, XMVECTOR direction, Hittable* target, float damage)
{
	this->damage = damage;
	this->target = target;
	this->position = position;
	this->velocity = direction * 10;

	active = true;
}

#pragma endregion

#pragma region Helpers

/*
Gets the y value that the cannonball will be beneath the water at.
*/
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

#pragma endregion

#pragma region Sound

/*
Plays a random cannonball splash sound, if any are not already playing.
*/
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

#pragma endregion