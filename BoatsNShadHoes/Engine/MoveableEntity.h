#ifndef MOVEABLE_ENTITY_H
#define MOVEABLE_ENTITY_H

#include "DrawableEntity.h"

class MoveableEntity : public DrawableEntity
{
public:
	MoveableEntity(Mesh*, Material*);
	~MoveableEntity(void);

	DirectX::XMVECTOR velocity;
	DirectX::XMVECTOR angularVelocity;

	DirectX::XMVECTOR acceleration;
	DirectX::XMVECTOR angularAcceleration;

	float friction;

	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(float dt, const XMMATRIX& parentMat);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

#endif

