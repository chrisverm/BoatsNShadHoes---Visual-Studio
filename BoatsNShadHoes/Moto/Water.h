#ifndef WATER_H
#define WATER_H

#include "DrawableEntity.h"
#include "ResourceManager.h"

class Water : public DrawableEntity
{
public:
	Water(std::string, std::string);
	~Water(void);
};

#endif