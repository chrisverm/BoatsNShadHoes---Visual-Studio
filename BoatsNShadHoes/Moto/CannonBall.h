#pragma once

#include "MoveableEntity.h"
#include "ResourceManager.h"

class CannonBall : public MoveableEntity
{
public:
	CannonBall(Mesh*, Material*, ID3D11RasterizerState*);
	~CannonBall(void);

	void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	void Update(float dt, const XMMATRIX& parentMat);

	void SetVel(XMVECTOR);

	void Fire(XMVECTOR position, XMVECTOR acceleration);

private:
	bool inPlay;

};