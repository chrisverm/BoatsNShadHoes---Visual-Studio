#ifndef MOVEABLE_ENTITY_H
#define MOVEABLE_ENTITY_H

#include "DrawableEntity.h"

class MoveableEntity : public DrawableEntity
{
public:
	MoveableEntity(Mesh*, Material*);
	~MoveableEntity(void);

	XMVECTOR velocity;
	XMVECTOR angularVelocity;
	float maxVel;

	XMVECTOR acceleration;
	XMVECTOR angularAcceleration;
	float maxAccel;

	float friction;

	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(float dt, const XMMATRIX& parentMat);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

#endif

