#ifndef MOVEABLE_ENTITY_H
#define MOVEABLE_ENTITY_H

#include "DrawableEntity.h"

class MoveableEntity : public DrawableEntity
{
public:
	XMVECTOR velocity; // should be moved into private

	const XMVECTOR& Velocity;
	const XMVECTOR& AngularVelocity;

protected:
	MoveableEntity(Mesh*, Material*);
	~MoveableEntity(void);
	
	float maxVel, maxAccel, friction;
	XMVECTOR acceleration;
	XMVECTOR angularAcceleration;

	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(float dt, const XMMATRIX& parentMat);
	virtual void Render(ID3D11DeviceContext* deviceContext);

private:
	
	XMVECTOR angularVelocity;

};

#endif

