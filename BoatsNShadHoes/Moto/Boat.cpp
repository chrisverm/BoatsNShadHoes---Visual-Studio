#include "Boat.h"
#include "Game.h"

std::vector<AudioPlayer*> Boat::hitSounds;
std::vector<AudioPlayer*> Boat::fireSounds;

#pragma region Constructor

/*
Constructor for a boat. 
Calls base Moveable Entity, along with setting stats for the boat.
Also sets things like maxVel/Accel, friction, bounding box, etc.
*/
Boat::Boat(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState, BOAT_STATS b) 
	: MoveableEntity(mesh, material, rasterizerState, depthStencilState)
{ 
	SetStats(b);

	// default stats for boat
	dead = false;
	maxVel = 3.0f;
	maxAccel = 2.0f;
	friction = 0.9f;

	position = XMVectorSetY(position, -0.5f);

	rotation = XMVectorSetZ(rotation, 0.0f);
	minAngleClamps = XMVectorSet(-360.0f, -360.0f, -10.0f, 0.0f) * XMConvertToRadians(1.0f); // deg * (pi / 180)
	maxAngleClamps = XMVectorSet( 360.0f,  360.0f,  10.0f, 0.0f) * XMConvertToRadians(1.0f);

	bounds = new Bounds(&position, XMFLOAT3(5,5,15));
}

#pragma endregion

#pragma region Update, Init

/*
Update.
Calls the unimplemented Move method (define in subclass).
Sinks if dead, otherwise positions itself on the water before updating Moveable.
*/
void Boat::Update(float dt, const XMMATRIX& parentMat)
{
	Move(dt);

	if (IsDead())
	{
		// apply downward position change to sunken boat
		position -= XMVectorSet(0.0, +0.001f, 0.0f, 0.0f);
	}
	else 
	{
		// set boat's Y position on the water
		float waveY  = GetYFromXZ(position);
		position = XMVectorSetY(position, waveY - 0.5f);

		// set boat's angle to appear on the water
		float frontY = GetYFromXZ(position + forward * 3);
		float backY  = GetYFromXZ(position - forward * 3);

		XMVECTOR frontForward = XMVectorSetY(forward, frontY);
		XMVECTOR backForward  = XMVectorSetY(-forward, backY);
		XMVECTOR newForward = frontForward - backForward;

		float angle = sin(XMVectorGetY(newForward) * (3.1415f / 180) / XMVectorGetX(XMVector3Length(newForward)));
		rotation = XMVectorSetX(rotation, -angle * 15);
	}

	MoveableEntity::Update(dt, parentMat);
}


/*
Sets the stats of the boat using the given struct
*/
void Boat::SetStats(BOAT_STATS b)
{
	stats.health		= b.health;
	stats.armor			= b.armor;
	stats.ammunition	= b.ammunition;
	stats.maxAmmunition	= b.maxAmmunition;
	stats.rateOfFire	= b.rateOfFire;
	stats.damage		= b.damage;
}

#pragma endregion

#pragma region Movement

/*
Not implemented, should be implemented in PlayerBoat/AIBoat for to define how this boat moves.
*/
void Boat::Move(float dt) { }  

/*
Causes the boat to accelerate forward.
*/
void Boat::MoveForward()
{
	acceleration = forward * 2.0f;
}

/*
Causes the boat to rotate to the right (lean to positive z).
TODO: The comment here is right, this needs to use dt rather than adding 0.0001.
*/
void Boat::PortHelm()
{
	rotation = XMVectorSetZ(rotation, XMVectorGetZ(rotation) + 0.0001f); // should be multipled by dt?
}

/*
Causes the boat to rotate to the left (lean to negative z).
TODO: The comment here is right, this needs to use dt rather than subtracting 0.0001.
*/
void Boat::StarboardHelm()
{
	rotation = XMVectorSetZ(rotation, XMVectorGetZ(rotation) - 0.0001f); // should be multipled by dt?
}

#pragma endregion

#pragma region Stats

/*
Returns the amount of ammunition left on the boat
*/
short Boat::Ammunition() const { return stats.ammunition; }

/*
Returns the maximum ammunition of the boat
*/
short Boat::MaximumAmmunition() const { return stats.maxAmmunition; }

/*
Returns the current health of the boat
*/
float Boat::Health() const { return stats.health; }

/*
Returns whether or not the boat is currently dead.
*/
bool Boat::IsDead() const { return dead; }

#pragma endregion

#pragma region Actions

/*
Adds ammunition to the boat.
Returns false if ammunition couldnt be added (already at max ammunition!).
*/
bool Boat::AddAmmunition(CannonBall* cball)
{
	if(stats.ammunition < stats.maxAmmunition)
	{
		// not at maximum ammunition
		cannonballs.push_back(cball);
		stats.ammunition += 1;
		
#if defined(DEBUG) | defined (_DEBUG)
		std::cout << "added ammo!" << std::endl;
#endif
		return true;
	}
	
#if defined(DEBUG) | defined (_DEBUG)
	std::cout << " could not add ammo! Deleting cannonball." << std::endl;
#endif
	delete cball;

	// at maximum ammunition; cannot add more
	return false;
}

/*
Fires a cannonball if there are any left.
Passes the target to the cannonball for collision detection.
Returns false if no cannonball could be fired (out of ammo).
*/
bool Boat::Fire(Boat* target)
{
	// check for inactive cannonballs
	for(short i = 0; i < stats.ammunition; i++)
	{
		if(!cannonballs[i]->Active())
		{
			cannonballs[i]->Fire(this->position + this->up * 2, (this->right + this->up), target, stats.damage);

#if defined(DEBUG) | defined(_DEBUG)
			std::cout << "Ammunition remaining : " << Ammunition() << std::endl;
#endif

#if defined SOUND_PLAY
			PlayCannonFire();
#endif
			// successfully fired!
			return true;
		}
	}

	// could not fire
	return false;
}

/*
Takes damage from a cannonball. 
Amount of damage taken is based on the enemy damage as well as this ship's armor.
*/
void Boat::TakeDamage(float amnt)
{
	stats.health -= (amnt-stats.armor);

	if(stats.health <= 0)
		dead = true;
}

#pragma endregion

#pragma region Hittable Implementation

/*
Called when a cannonball detects collision with this boat.
This boat will now take the damage attached to that cannonball.
*/
void Boat::Hit(float damage)
{
	TakeDamage(damage);

#if defined(DEBUG) | defined(_DEBUG)
	std::cout << "Other boat's hp left: " << Health() << std::endl;
#endif

#if defined SOUND_PLAY
	PlayHitSound();
#endif
}

/*
Gets a pointer to the Bounds of this hittable object (boat in this case).
*/
Bounds* Boat::GetBoundsPtr()
{ return bounds; }

#pragma endregion

#pragma region Helpers

/*
Gets the y position the boat should be at to be on the water based on the position its at now.
*/
float Boat::GetYFromXZ(XMVECTOR pos)
{
	float start = (XMVectorGetX(pos) / 150.0f) * 360;
	float degsToRads = 3.1415f / 180.0f;

	float angle = (start + Game::vsPerFrameData->time * 200) * 3.1415f;
	float amplitude = 2.0f;
	float frequency = 0.25f;

	angle *= frequency;

	return sin(angle * degsToRads) * amplitude;
}

#pragma endregion

#pragma region Sounds
#if defined(SOUND_PLAY)

/*
Plays a random cannon fire sound, as long as there is one not already playing.
*/
void Boat::PlayCannonFire()
{
	std::vector<AudioPlayer*> available;

	for (std::vector<AudioPlayer*>::iterator it = fireSounds.begin(); it != fireSounds.end(); it++)
	{ if (!(*it)->playing()) available.push_back((*it)); }

	if (available.size() == 0)
		return;

	int index = rand() % available.size();

	XMFLOAT3 blah;
	XMStoreFloat3(&blah, position);

	available[index]->play();
	available[index]->setPosition(blah.x, blah.y, blah.z);
}

/*
Plays a random cannon hit sound, as long as there is one not already playing.
*/
void Boat::PlayHitSound()
{
	std::vector<AudioPlayer*> available;

	for (std::vector<AudioPlayer*>::iterator it = hitSounds.begin(); it != hitSounds.end(); it++)
	{ if (!(*it)->playing()) available.push_back((*it)); }

	if (available.size() == 0)
		return;

	int index = rand() % available.size();

	XMFLOAT3 blah;
	XMStoreFloat3(&blah, position);

	available[index]->play();
	available[index]->setPosition(blah.x, blah.y, blah.z);

}

#endif
#pragma endregion
