#ifndef BOATSNSHADHOES_CANNON_H
#define BOATSNSHADHOES_CANNON_H

#include "DrawableEntity.h"
#include "CannonBall.h"
#include "Hittable.h"

class Cannon :
	public DrawableEntity
{
public:
	Cannon(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);

	void Fire(CannonBall*, Hittable*, XMVECTOR, float);
};

#endif

