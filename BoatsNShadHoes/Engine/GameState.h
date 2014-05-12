#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include "ResourceManager.h"
#include "CameraManager.h"

class GameState
{
public:
	GameState(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		this->device = device;
		this->deviceContext = deviceContext;
	}
	virtual ~GameState() { }
	virtual bool Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw(float dt) = 0;

	void Unload() { this->~GameState(); }

protected:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

};

#endif