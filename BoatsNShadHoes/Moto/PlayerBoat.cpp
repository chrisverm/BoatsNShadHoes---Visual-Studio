#include "PlayerBoat.h"

/*
Does nothing more than Boat's update.
*/
PlayerBoat::PlayerBoat(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState, BOAT_STATS stats) :
	Boat(mesh, mat, rastState, depthStenState, stats)
{ }

/*
Turns and moves based on keyboard input (WAD).
*/
void PlayerBoat::Move(float dt)
{
	bool turning = false;

	if (Input::KeyDown('W'))
	{
		MoveForward();
	}
	if (Input::KeyDown('A'))
	{
		turning = true;
		PortHelm();
	}
	if (Input::KeyDown('D'))
	{
		turning = true;
		StarboardHelm();
	}

	if (!turning) 
	{
		rotation = XMVectorSetZ(rotation, XMVectorGetZ(rotation) - XMVectorGetZ(rotation) * 0.975f * dt);

		if (abs(XMVectorGetZ(rotation)) < 0.01f) rotation = XMVectorSetZ(rotation, 0.0f);
	}

	angularAcceleration = XMVectorSet(0.0f, -XMVectorGetZ(rotation), 0.0f, 0.0f);

	/* This removed the ability to turn while not moving
	angularAcceleration = XMVectorSet(0.0f, -roll, 0.0f, 0.0f);
	angularAcceleration = XMVector3Normalize(angularAcceleration);
	angularAcceleration *= XMVector3Length(velocity) / 25.0f;*/
}