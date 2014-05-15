#ifndef CANNONBALL_H
#define CANNONBALL_H

#include "MoveableEntity.h"
#include "Hittable.h"
#include "ResourceManager.h"

class Game;

class CannonBall : public MoveableEntity
{
public:
	CannonBall(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);
	~CannonBall(void);

	void Update(float dt, const XMMATRIX& parentMat);

	void SetVelocity(XMVECTOR);
	bool Active() const;

	void Fire(XMVECTOR position, XMVECTOR acceleration, Hittable* target, float damage);

private:
	bool active;
	float damage;

	Hittable* target;

	float GetYFromXZ(XMVECTOR pos, float time);
};
#endif