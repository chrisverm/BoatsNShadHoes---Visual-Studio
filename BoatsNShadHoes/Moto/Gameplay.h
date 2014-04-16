#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <vector>
#include "GameState.h"
#include "Entity.h"

class Gameplay : public GameState
{
public:
	Gameplay();
	~Gameplay();
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update(float dt);
	void Draw(float dt);

private:
	std::vector<Entity*> entities;

};

#endif