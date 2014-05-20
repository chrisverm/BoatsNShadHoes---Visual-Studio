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
	sunk = dead = false;
	maxVel = 3.0f;
	maxAccel = 2.0f;
	friction = 0.9f;

	angularVelocityClamps = XMVectorSet(0,1,0.05f,0);
	position = XMVectorSetY(position, -0.5f);

	rotation = XMVectorSetZ(rotation, 0.0f);
	minAngleClamps = XMVectorSet(-360.0f, -360.0f, -10.0f, 0.0f) * XMConvertToRadians(1.0f); // deg * (pi / 180)
	maxAngleClamps = XMVectorSet( 360.0f,  360.0f,  10.0f, 0.0f) * XMConvertToRadians(1.0f);

	bounds = new Bounds(&position, XMFLOAT3(5,5,15));
}

/*
Destroys a boat!
*/
Boat::~Boat()
{ MoveableEntity::~MoveableEntity(); }

#pragma endregion

#pragma region Update, Init

/*
Update.
Calls the unimplemented Move method (define in subclass).
Sinks if dead, otherwise positions itself on the water before updating Moveable.
*/
void Boat::Update(float dt, const XMMATRIX& parentMat)
{
	if (!Dead())
	{
		Move(dt);
		angularVelocity = XMVectorClamp(angularVelocity, -angularVelocityClamps, angularVelocityClamps);

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

	if (!Sunk())
	{
		MoveableEntity::Update(dt, parentMat);
		if (XMVectorGetY(position) < GetYFromXZ(position) - 10)
			sunk = true;
	}
}
void Boat::Render(ID3D11DeviceContext* deviceContext)
{
	if (!Sunk())
	{ MoveableEntity::Render(deviceContext); }
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
void Boat::Move(float dt)
{
	// (actually turns the boat(on its y) based on how far its rolled to one side(on its z)).
	angularAcceleration = XMVectorSetY(angularAcceleration, -XMVectorGetZ(rotation));
}  

/*
Causes the boat to accelerate forward.
*/
void Boat::MoveForward()
{ acceleration = forward * 2.0f; }

/*
Stops moving forward (has to wait for forward velocity to die down to friction, but stops accelerating forward).
*/
void Boat::Stall()
{ acceleration = XMVectorZero(); }

/*
Causes the boat to rotate to the right (lean to negative x). Starboard side
*/
void Boat::RudderRight()
{ angularAcceleration = XMVectorSet(0,0,-0.25f,0); }

/*
Causes the boat to rotate to the left (lean to positive x). Port side
*/
void Boat::RudderLeft()
{ angularAcceleration = XMVectorSet(0,0,0.25f,0); }

/*
Causes the boat to rotate back to upright.
Will overshoot its mark cause it usues acceleration, but i think its more accurate to be honest.
*/
void Boat::StallRudder()
{ angularAcceleration = XMVectorSet(0,0,1,0) * -XMVectorGetZ(rotation); }

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
bool Boat::Dead() const { return dead; }

/*
Returns whether or not the boat is currently sunk.
*/
bool Boat::Sunk() const { return sunk; }

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
		
		return true;
	}
	
	delete cball;

	// at maximum ammunition; cannot add more
	return false;
}

/*
Fires all the cannons on the left side that we have ammo to fire with.
*/
void Boat::FireLeftCannons()
{
	for (std::vector<Cannon*>::iterator it = leftCannons.begin(); it != leftCannons.end(); it++)
	{
		if (!Fire(*it)) return; // Firing on one cannon failed, dont bother with the others.
	}
}

/*
Fires all the cannons on the right side that we have ammo to fire with.
*/
void Boat::FireRightCannons()
{
	for (std::vector<Cannon*>::iterator it = rightCannons.begin(); it != rightCannons.end(); it++)
	{
		if (!Fire(*it)) return; // Firing on one cannon failed, dont bother with the others.
	}
}

/*
Fires all the cannons on both sides.
If not enough cannonballs, it will fire still fire equal ammounts on either side.
Unless we have an odd number of cannonballs, then the right side will have one more than the left.
*/
void Boat::FireAllCannons()
{
	for (int i = 0; i < (int)rightCannons.size(); i++)
	{
		// Hopefuly we have the same number of cannons.
		if (!Fire(rightCannons[i])) return;
		if (!Fire(leftCannons[i])) return;
	}
}

/*
Fires a cannonball if there are any left.
Passes the target to the cannonball for collision detection.
Returns false if no cannonball could be fired (out of ammo).
*/
bool Boat::Fire(Cannon* cannon)
{
	// check for inactive cannonballs
	for(short i = 0; i < stats.ammunition; i++)
	{
		if(!cannonballs[i]->Active())
		{
			cannon->Fire(cannonballs[i], targets, velocity, stats.damage);

#if defined(DEBUG) | defined(_DEBUG)
			//std::cout << "Ammunition remaining : " << Ammunition() << std::endl;
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
	{
		acceleration = XMVectorSet(0,-9.81f,0,0);
		angularAcceleration = XMVectorSet(0,0,0,0);
		dead = true;
	}
}

/*
Adds a Cannon to the specified side.
*/
void Boat::AddCannon(Cannon* cannon, bool rightSide)
{
	int side;
	int number;
	if (rightSide)
	{
		side = 1;
		number = rightCannons.size();
		rightCannons.push_back(cannon);
	}
	else
	{
		side = -1;
		number = leftCannons.size();
		leftCannons.push_back(cannon);
	}

	// Position
	cannon->rotation = XMVectorSet(0,(3.14f/2.0f) * side, 0, 0);
	cannon->position = position
		+ up * 2.7f //y
		+ right * (float)side * 1.5f // x
		- forward * 2.0f + forward * (float)number * 3.0f; //z

	CannonStats cannonStats;
	cannonStats.coolDownTime = stats.rateOfFire;
	cannonStats.redTint = 0.3f;
	cannon->SetStats(cannonStats);
	
	this->AddChild(cannon);
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
	// Uncomment small block in line below for the fish.
	float start = (XMVectorGetX(pos) /* + 0 */ / 150.0f) * 360;
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
