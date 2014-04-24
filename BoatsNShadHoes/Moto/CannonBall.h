#pragma once

#include "MoveableEntity.h"
#include "ResourceManager.h"

class CannonBall : public MoveableEntity
{
public:
	CannonBall(std::string, std::string);
	~CannonBall(void);

	void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	void Update(float dt, const XMMATRIX& parentMat);

	void SetVel(XMVECTOR);
};

