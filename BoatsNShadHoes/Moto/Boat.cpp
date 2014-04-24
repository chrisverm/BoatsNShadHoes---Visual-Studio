#include "Boat.h"
#include "InputManager.h"

using namespace DirectX;
Boat::Boat(std::string meshId, std::string matId, BOAT_STATS b, bool controllable) 
	: MoveableEntity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{ 
	this->controllable = controllable;

	SetStats(b);

	// default stats for boat
	dead = false;
	maxVel = 3.0f;
	maxAccel = 2.0f;
	friction = 0.9f;
	roll = 0.0f;
	maxRoll = 10.0f;
}

Boat::~Boat()
{ }

void Boat::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void Boat::Update(float dt, const XMMATRIX& parentMat)
{
	if (controllable) Move();

	MoveableEntity::Update(dt, parentMat);

	if (IsDead())
	{
		// apply downward position change to sunken boat
		position -= XMVectorSet(0.0, +0.001f, 0.0f, 0.0f);
	}
}

void Boat::Move()
{
	if (Input::KeyDown('W'))
	{
		MoveForward();
	}
	if (Input::KeyDown('A'))
	{
		PortHelm();
	}
	if (Input::KeyDown('D'))
	{
		StarboardHelm();
	}
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
	//Yaw Yaw Yaw YEET
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
 * Returns the current health of the boat
 *
 * @return	Current health of the boat
 */
float Boat::Health() const { return stats.health; }

bool Boat::IsDead() const { return dead; }

void Boat::MoveForward()
{
	//angularVelocity += forward;
	acceleration = XMVECTOR(Forward) * 2.0f;
}

void Boat::PortHelm()
{
	roll += 0.01f; // should be multipled by dt
	angularAcceleration = -XMVECTOR(Up) / 25.0f;
}

void Boat::StarboardHelm()
{
	roll -= 0.01f; // should be multipled by dt
	angularAcceleration = XMVECTOR(Up) / 25.0f;
}

/*
 * Fires a cannonball and deals damage to the target boat
 *
 * @param	target	the boat which will be taking damage as a result of the cannon fire
 *
 * @return	none
 */
void Boat::Fire(Boat* target)
{
	// can only fire if ammo is available
	if(stats.ammunition > 0)
	{
		// TODO: spawn cannonball here

		target->TakeDamage(stats.damage);
		stats.ammunition -= 1;
	}
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