#ifndef BOAT_H
#define BOAT_H

#include "Entity.h"
#include "ResourceManager.h"

class Boat 
	: public Entity
{
public:
	Boat(std::string, std::string);
	~Boat();
};

#endif