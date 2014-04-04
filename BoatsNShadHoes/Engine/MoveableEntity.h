#ifndef MOVEABLE_ENTITY_H
#define MOVEABLE_ENTITY_H

#include "DrawableEntity.h"

class MoveableEntity : public DrawableEntity
{
public:
	MoveableEntity(Mesh*, Material*);
	~MoveableEntity(void);

	DirectX::XMVECTOR velocity;
	DirectX::XMVECTOR angualrVelocity;

	DirectX::XMVECTOR acceleration;
	DirectX::XMVECTOR angularAcceleration;

	float friction;

	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(ID3D11DeviceContext* deviceContext, float dt);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

#endif

