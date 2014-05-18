#include "PlayerBoat.h"

/*
Does nothing more than Boat's update.
*/
PlayerBoat::PlayerBoat(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState, BOAT_STATS stats) :
	Boat(mesh, mat, rastState, depthStenState, stats)
{ }

/*
Handles input from the player to move the boat / fire cannons.
*/
void PlayerBoat::HandleInput()
{
	if (Input::KeyDown('W')) 
		MoveForward(); 
	else
		Stop();

	if (Input::KeyDown('A')) // left
		PortHelm(); 
	else if (Input::KeyDown('D')) // right
		StarboardHelm(); 
	else 
		StallSteering();

	// Toggle Selected sides (We should probably have some sort of graphic (color overlay?) on selected cannons).
	if (Input::KeyUp('Q')) 
		leftSelected = !leftSelected; 
	if (Input::KeyUp('E')) 
		rightSelected = !rightSelected; 

	if (Input::KeyUp(' '))
	{
		if (leftSelected)
			FireLeftCannons(target);
		if (rightSelected)
			FireRightCannons(target);
	}
}

/*
Turns and moves based on keyboard input (WAD).
*/
void PlayerBoat::Move(float dt)
{
	HandleInput();

	// (actually turns the boat(on its y) based on how far its rolled to one side(on its z)).
	angularAcceleration = XMVectorSetY(angularAcceleration, -XMVectorGetZ(rotation));

	/* This removed the ability to turn while not moving, but it might not any longer with some new changes.
	angularAcceleration = XMVectorSet(0.0f, -roll, 0.0f, 0.0f);
	angularAcceleration = XMVector3Normalize(angularAcceleration);
	angularAcceleration *= XMVector3Length(velocity) / 25.0f;*/
}