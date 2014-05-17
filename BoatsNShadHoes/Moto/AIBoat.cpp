#include "AIBoat.h"

/*
Does nothing more than Boat's update.
*/
AIBoat::AIBoat(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState, BOAT_STATS stats) :
	Boat(mesh, mat, rastState, depthStenState, stats)
{
}

/*
NOT IMPLEMENTED, GAAAHHHHHHHHHHHHHH
*/
void AIBoat::Move(float dt)
{
}