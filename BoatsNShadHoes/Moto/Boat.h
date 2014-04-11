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
	void Update(ID3D11DeviceContext* deviceContext, float dt);

	void SetPosition(float x, float y, float z);
	void SetRotation(float r, float p, float y);

	void MoveForward();
	void PortHelm();
	void StarboardHelm();
};

#endif