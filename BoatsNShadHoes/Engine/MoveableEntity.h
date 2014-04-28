#ifndef MOVEABLE_ENTITY_H
#define MOVEABLE_ENTITY_H

#include "DrawableEntity.h"

class MoveableEntity : public DrawableEntity
{
public:
	MoveableEntity(Mesh*, Material*);
	~MoveableEntity(void);

	const XMVECTOR& Velocity;
	const XMVECTOR& AngularVelocity;

	// Init, Update, Render
	virtual void Update(float dt, const XMMATRIX& parentMat);

protected:
	// Physics stuff.
	float maxVel, maxAccel, friction;
	XMVECTOR velocity;
	XMVECTOR angularVelocity;
	XMVECTOR acceleration;
	XMVECTOR angularAcceleration;

private:

};

#endif

