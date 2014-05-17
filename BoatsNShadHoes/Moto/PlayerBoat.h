#ifndef BOATSNSHADHOES_PLAYERBOAT_H
#define BOATSNSHADHOES_PLAYERBOAT_H

#include "boat.h"

class PlayerBoat : public Boat
{
public:
	PlayerBoat(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*, BOAT_STATS);

	void Move(float);
};

#endif

