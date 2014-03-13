#ifndef WATER_H
#define WATER_H

#include "Entity.h"
#include "ResourceManager.h"

class Water
	: public Entity
{
public:
	Water(std::string, std::string);
	~Water(void);
};

#endif