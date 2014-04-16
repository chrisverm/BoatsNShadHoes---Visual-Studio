#include "Gameplay.h"

Gameplay::Gameplay()
{

}

Gameplay::~Gameplay()
{
	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end() ; it++)
	{ delete (*it); }
}

bool Gameplay::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if (!GameState::Initialize(device, deviceContext))
		return false;

	return true;
}

void Gameplay::Update(float dt)
{

}

void Gameplay::Draw(float dt)
{

}