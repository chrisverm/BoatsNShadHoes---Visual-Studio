#pragma once

#include "MoveableEntity.h"
#include "ResourceManager.h"

class CannonBall : public MoveableEntity
{
public:
	CannonBall(std::string, std::string);
	~CannonBall(void);

	void SetVel(XMVECTOR);
};

