#include "AIBoat.h"

/*
Does nothing more than Boat's update.
*/
AIBoat::AIBoat(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState, BOAT_STATS stats) :
	Boat(mesh, mat, rastState, depthStenState, stats)
{
}

/*
I'M TRYING TO IMPLEMENT NOW GAHHHHHHH -Love, Justin
*/
void AIBoat::Move(float dt)
{
	Boat* player = (Boat*)targets[0];
	
	XMVECTOR enemyPos = player->position;
	if(XMVectorGetX(enemyPos) < XMVectorGetX(position))
	{
		RudderLeft();
	}
	else if(XMVectorGetX(enemyPos) < XMVectorGetX(position))
	{
		RudderRight();
	}
}