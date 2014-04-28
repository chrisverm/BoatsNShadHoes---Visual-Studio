#ifndef BOAT_H
#define BOAT_H

#include "MoveableEntity.h"
#include "ResourceManager.h"
#include "InputManager.h"

/*
 * Basic stats that each boat will have
 */
struct BOAT_STATS
{
	float health;			// amount of health points of the boat
	float armor;			// armor value of the boat (reduces damage taken by a flat amount)
	short ammunition;		// amount of ammunition the boat has
	float rateOfFire;		// rate at which the boat can fire cannonballs (per second)
	float damage;			// base damage of cannonball fire for the ship
	//float maxVelocity;	// maximum velocity of the boat
	//float maxTurnRate;	// maximum turn rate of the boat
	//short maxAmmunition;	// maximum ammunition count for the boat
	//short penetrationDamage;	// amount of enemy armor that is ignored upon hit

	BOAT_STATS() { health = armor = rateOfFire = 0.0f; ammunition = 0; } // 0 defaults
	~BOAT_STATS() {}
};

class Boat : public MoveableEntity
{
public:
	Boat(Mesh*, Material*, ID3D11RasterizerState*, BOAT_STATS, bool controllable);
	~Boat();

	void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	void Update(float dt, const XMMATRIX& parentMat);

	void Move(float dt);

	void SetPosition(float x, float y, float z);
	void SetRotation(float r, float p, float y);
	void SetStats(BOAT_STATS);

	short Ammunition() const;
	float Health() const;
	bool IsDead() const;

	void MoveForward();
	void PortHelm();
	void StarboardHelm();
	void Fire(Boat*); // testing purposes!
	void TakeDamage(float);

private:
	BOAT_STATS stats;
	bool dead, controllable;

};

#endif