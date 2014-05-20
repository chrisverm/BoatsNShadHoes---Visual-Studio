#include "AIBoat.h"

/*
Does nothing more than Boat's update.
*/
AIBoat::AIBoat(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState, BOAT_STATS stats) :
	Boat(mesh, mat, rastState, depthStenState, stats)
{ }

/*
I'M TRYING TO IMPLEMENT NOW GAHHHHHHH -Love, Justin
YAY!
*/
void AIBoat::Move(float dt)
{
	Boat* player = (Boat*)targets[0];
	
	XMVECTOR playerPos = player->position;
	XMVECTOR differenceVector = playerPos - position;

	//Gets angle between AIboat and playerBoat. In radians
	float frontAngle = XMVectorGetX(XMVector3AngleBetweenVectors(forward, differenceVector));
	float sideAngle = XMVectorGetX(XMVector3AngleBetweenVectors(right, differenceVector));

	float distance = XMVectorGetX(XMVector3Length(differenceVector));
		
	// Always move forward
	MoveForward();
	StallRudder();

	// Too far away, go to him.
	if(distance > 50)
	{
		if (frontAngle > 0.2f) // If this fails, just go forward.
		{
			// turn to the right
			if (sideAngle < 3.14f / 2.0f)
			{ RudderRight(); }
			// turn to the left
			else 
			{ RudderLeft(); }
		}
	}
	// Too close, turn away
	else if (distance < 20)
	{
		// turn to the right
		
		// if closer to 0, turn to go 0, (
		// if closer to pi, turn to go pi..
		if (sideAngle < 3.14f / 2.0f)
		{ RudderLeft(); }
		// turn to the left
		else 
		{ RudderRight(); }
	}
	else
	{
		// try to get in better position to shoot
		// if player boat in back right or front left quadrant...
		if ((sideAngle < 3.14f / 2.0f && frontAngle > 3.14f / 2.0f) || (sideAngle > 3.14f / 2.0f && frontAngle < 3.14f / 2.0f))
		{ RudderRight(); }
		// if player boat in back left or front right quadrant...
		else if ((sideAngle > 3.14f / 2.0f && frontAngle > 3.14f / 2.0f) || (sideAngle < 3.14f / 2.0f && frontAngle < 3.14f / 2.0f))
		{ RudderLeft(); }
	}

	// check if we can fire.
	// if side angle is close to 0 or pi, fire on that side...
	if (distance < 80)
	{
		if(sideAngle <= 3.14f/4.0f)
		{ FireRightCannons(); }
		if(sideAngle >= 3.0f*(3.14f/4.0f))
		{ FireLeftCannons(); }
	}

	Boat::Move(dt);

}