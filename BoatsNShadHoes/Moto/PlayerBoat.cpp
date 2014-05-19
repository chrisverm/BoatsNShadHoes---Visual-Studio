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
		Stall();

	if (Input::KeyDown('A')) // left
		RudderLeft(); 
	else if (Input::KeyDown('D')) // right
		RudderRight(); 
	else 
		StallRudder();

	if (Input::KeyUp('Q') || Input::MouseButtonUp(LeftMB)) 
		FireLeftCannons();
	if (Input::KeyUp('E') || Input::MouseButtonUp(RightMB)) 
		FireRightCannons(); 
	if (Input::KeyUp(' ') || Input::MouseButtonUp(MiddleMB)) 
		FireAllCannons(); 
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