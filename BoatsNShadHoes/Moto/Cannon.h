#ifndef BOATSNSHADHOES_CANNON_H
#define BOATSNSHADHOES_CANNON_H

#include "DrawableEntity.h"
#include "CannonBall.h"
#include "Hittable.h"

struct CannonStats
{
	float coolDownTime;
	float redTint;
};

class Cannon :
	public DrawableEntity
{
public:
	Cannon(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);
	void SetStats(CannonStats);

	float coolDownTime;

	void Update(float, const XMMATRIX&);
	void Fire(CannonBall*, std::vector<Hittable*>, XMVECTOR, float);

private:
	bool active;
	float coolDown;
	float redTint;
};

#endif

