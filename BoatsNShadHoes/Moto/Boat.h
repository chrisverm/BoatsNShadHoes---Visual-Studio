#ifndef BOAT_H
#define BOAT_H

#include "MoveableEntity.h"
#include "ResourceManager.h"

class Boat 
	: public MoveableEntity
{
public:
	Boat(std::string, std::string);
	~Boat();

	void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
};

#endif