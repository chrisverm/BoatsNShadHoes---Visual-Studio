#ifndef BOATSNSHADHOES_AIBOAT_H
#define BOATSNSHADHOES_AIBOAT_H

#include "boat.h"

class AIBoat : public Boat
{
public:
	AIBoat(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*, BOAT_STATS);

	void Move(float);
};

#endif

