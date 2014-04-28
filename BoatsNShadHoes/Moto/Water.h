#ifndef WATER_H
#define WATER_H

#include "DrawableEntity.h"
#include "ResourceManager.h"

class Water : public DrawableEntity
{
public:
	Water(Mesh*, Material*, ID3D11RasterizerState*);
	~Water(void);
};

#endif