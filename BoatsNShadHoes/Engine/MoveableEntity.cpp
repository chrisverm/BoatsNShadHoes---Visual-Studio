#include "MoveableEntity.h"

using namespace DirectX;

#pragma region Constructor/Destructor

/*
Constructor

Uses parents constructor.
Requires a Mesh and a Material to draw with.
Initializes physics values (vel, angvel, accel, frict, ect).
*/
MoveableEntity::MoveableEntity(Mesh* mesh, Material*mat)
	: DrawableEntity(mesh,mat), Velocity(velocity), AngularVelocity(angularVelocity)
{
	velocity = XMVectorSet(0,0,0,0);
	angularVelocity = XMVectorSet(0,0,0,0);
	maxVel = 0;

	acceleration = XMVectorSet(0,0,0,0);
	angularAcceleration = XMVectorSet(0,0,0,0);
	maxAccel = 0;

	friction = 0;
}

/*
Destructor, loops through children and deletes them, (ignores cameras because they're handled in camera manager).
*/
MoveableEntity::~MoveableEntity(void)
{ DrawableEntity::~DrawableEntity(); }

#pragma endregion

#pragma region Init, Update, Render

/*
Updates this entity to the current timestep.
Needs a reference to the parents world matrix.

Performs velocity/acceleration calculations (integration).
*/
void MoveableEntity::Update(float dt, const XMMATRIX& parentMat)
{
	acceleration = XMVector3ClampLength(acceleration, 0, maxAccel);

	velocity += acceleration * dt;
	angularVelocity += angularAcceleration * dt;

	velocity = XMVector3ClampLength(velocity, 0, maxVel);

	velocity -= velocity * friction * dt;
	angularVelocity -= angularVelocity * friction * dt;
	
	position += velocity * dt;
	rotation += angularVelocity * dt;

	acceleration = XMVectorZero();
	angularAcceleration = XMVectorZero();

	DrawableEntity::Update(dt, parentMat);
}

#pragma endregion
