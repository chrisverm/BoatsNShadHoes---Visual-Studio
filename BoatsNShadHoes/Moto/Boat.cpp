#include "Boat.h"
#include "InputManager.h"

using namespace DirectX;
Boat::Boat(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState, BOAT_STATS b, bool controllable) 
	: MoveableEntity(mesh, material, rasterizerState, depthStencilState)
{ 
	this->controllable = controllable;

	SetStats(b);

	// default stats for boat
	dead = false;
	maxVel = 3.0f;
	maxAccel = 2.0f;
	friction = 0.9f;

	rotation = XMVectorSetZ(rotation, 0.0f);
	minAngleClamps = XMVectorSet(-360.0f, -360.0f, -10.0f, 0.0f) * XMConvertToRadians(1.0f); // deg * (pi / 180)
	maxAngleClamps = XMVectorSet( 360.0f,  360.0f,  10.0f, 0.0f) * XMConvertToRadians(1.0f);
}

Boat::~Boat()
{ }

void Boat::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void Boat::Update(float dt, const XMMATRIX& parentMat)
{
	if (controllable) Move(dt);

	MoveableEntity::Update(dt, parentMat);

	if (IsDead())
	{
		// apply downward position change to sunken boat
		position -= XMVectorSet(0.0, +0.001f, 0.0f, 0.0f);
	}
}

void Boat::Move(float dt)
{
	bool turning = false;

	if (Input::KeyDown('W'))
	{
		MoveForward();
	}
	if (Input::KeyDown('A'))
	{
		turning = true;
		PortHelm();
	}
	if (Input::KeyDown('D'))
	{
		turning = true;
		StarboardHelm();
	}

	if (!turning) 
	{
		rotation = XMVectorSetZ(rotation, XMVectorGetZ(rotation) - XMVectorGetZ(rotation) * 0.975f * dt);

		if (abs(XMVectorGetZ(rotation)) < 0.01f) rotation = XMVectorSetZ(rotation, 0.0f);
	}

	angularAcceleration = XMVectorSet(0.0f, -XMVectorGetZ(rotation), 0.0f, 0.0f);

	/* This removed the ability to turn while not moving
	angularAcceleration = XMVectorSet(0.0f, -roll, 0.0f, 0.0f);
	angularAcceleration = XMVector3Normalize(angularAcceleration);
	angularAcceleration *= XMVector3Length(velocity) / 25.0f;*/
}

/*
 * Sets the stats of the boat using the given struct
 *
 * @param	b		the struct containing all of the stats of the boat
 *
 * @return	none
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

void Boat::SetPosition(float x, float y, float z)
{
	XMFLOAT3 pos(x, y, z);
	position = XMLoadFloat3(&pos);
}

void Boat::SetRotation(float roll, float pitch, float yaw)
{
	XMFLOAT3 rot(roll, pitch, yaw);
	rotation = XMLoadFloat3(&rot);
	UpdateForwardFromRotation();
}

/*
 * Returns the amount of ammunition left on the boat
 *
 * @return	Ammunition left in the boat
 */
short Boat::Ammunition() const { return stats.ammunition; }

/*
 * Returns the maximum ammunition of the boat
 *
 * @return	Maximum ammunition of the boat
 */
short Boat::MaximumAmmunition() const { return stats.maxAmmunition; }

/*
 * Returns the current health of the boat
 *
 * @return	Current health of the boat
 */
float Boat::Health() const { return stats.health; }

bool Boat::IsDead() const { return dead; }

/*
 * Adds ammunition to the boat.
 * If the ammunition is at maximum capacity, the ammunition cannot be added.
 *
 * @return	If the ammunition was successfully added
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

	// at maximum ammunition; cannot add more
	return false;
}

void Boat::MoveForward()
{
	acceleration = forward * 2.0f;
}

void Boat::PortHelm()
{
	rotation = XMVectorSetZ(rotation, XMVectorGetZ(rotation) + 0.0001f); // should be multipled by dt?
}

void Boat::StarboardHelm()
{
	rotation = XMVectorSetZ(rotation, XMVectorGetZ(rotation) - 0.0001f); // should be multipled by dt?
}

/*
 * Fires a cannonball and deals damage to the target boat
 *
 * @param	target	the boat which will be taking damage as a result of the cannon fire
 *
 * @return	none
 */
bool Boat::Fire(Boat* target)
{
	// check for inactive cannonballs
	for(short i = 0; i < stats.ammunition; i++)
	{
		if(!cannonballs[i]->Active())
		{
			cannonballs[i]->Fire(this->position, (this->right + this->up));
			target->TakeDamage(stats.damage);

			// successfully fired!
			return true;
		}
	}

	// could not fire
	return false;
}

/*
 * Takes damage from a cannonball. Amount of damage taken is based on the enemy damage as well as
 * this ship's armor
 *
 * @param	amnt	the amount of damage that the enemy ship deals
 *
 * @return	none
 */
void Boat::TakeDamage(float amnt)
{
	stats.health -= (amnt-stats.armor);

	if(stats.health <= 0)
		dead = true;
}