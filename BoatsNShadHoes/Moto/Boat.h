#ifndef BOAT_H
#define BOAT_H

#include "MoveableEntity.h"
#include "Hittable.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "CannonBall.h"
#include "Cannon.h"

class Game;

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
	short maxAmmunition;	// maximum ammunition count for the boat
	//short penetrationDamage;	// amount of enemy armor that is ignored upon hit

	BOAT_STATS() { health = armor = rateOfFire = 0.0f; ammunition = 0; } // 0 defaults
	~BOAT_STATS() {}
};

class Boat : public MoveableEntity, public Hittable
{
public:
	Boat(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*, BOAT_STATS);
	virtual ~Boat();

	// Update / Init
	void Update(float dt, const XMMATRIX& parentMat);
	void SetStats(BOAT_STATS);

	// Movement
	void virtual Move(float dt);
	void MoveForward();
	void Stall();
	void RudderRight();
	void RudderLeft();
	void StallRudder();

	// Stats
	short Ammunition() const;
	short MaximumAmmunition() const;
	float Health() const;
	bool IsDead() const;

	// Actions
	bool AddAmmunition(CannonBall*);
	void AddCannon(Cannon*, bool);
	void FireLeftCannons(Hittable*);
	void FireRightCannons(Hittable*);
	bool Fire(Hittable*, Cannon*);
	void TakeDamage(float);
	
	// Hittable implementation
	void Hit(float);
	Bounds* GetBoundsPtr();

	// Shared hit/fire sounds to use.
	static std::vector<AudioPlayer*> hitSounds;
	static std::vector<AudioPlayer*> fireSounds;

private:
	BOAT_STATS stats;
	bool dead;
	XMVECTOR angularVelocityClamps;

	std::vector<CannonBall*> cannonballs;
	std::vector<Cannon*> leftCannons;
	std::vector<Cannon*> rightCannons;

	// Positioning helper.
	float GetYFromXZ(XMVECTOR pos);

	// Sounds
	void PlayCannonFire();
	void PlayHitSound();
};

#endif